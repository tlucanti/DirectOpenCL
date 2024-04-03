
#include <guilib.h>

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#ifndef CONFIG_IMAGE_COMPRESS_AMOUNT
# define CONFIG_IMAGE_COMPRESS_AMOUNT 0
#endif

static int g_server;

static void image_compress_0(unsigned *image, unsigned long size,
			     unsigned char *dst, unsigned long *dst_size)
{
        for (unsigned long i = 0; i < size; i++) {
                dst[i * 3 + 0] = (image[i] & 0xFF0000) >> 16u;  // red
                dst[i * 3 + 1] = (image[i] & 0x00FF00) >> 8u;   // green
                dst[i * 3 + 2] = (image[i] & 0x0000FF);         // blue
        }

        *dst_size = size * 3;
}

static char image_compress(unsigned *image, unsigned long size,
			   unsigned char *dst, unsigned long *dst_size)
{
        switch (CONFIG_IMAGE_COMPRESS_AMOUNT) {
        case 0:
                image_compress_0(image, size, dst, dst_size);
                return '0';
        default:
                abort();
        }
}

static void sig_handler(int sig)
{
        if (sig == SIGPIPE) {
                fprintf(stderr, "server: got SIGPIPE\n");
                abort();
        } else {
                abort();
        }
}

static void *key_reader_thread(void *winptr)
{
        struct gui_window *window = winptr;
        char event;

        while (window->__key_reader_run) {
                event = soc_recv_char(&window->__client);

                switch (event) {
                case 'K':
                case 'k': {
                        bool pressed = (event == 'K');
                        int key = soc_recv_number(&window->__client);
                        printf("server: key %d %s\n", key, pressed ? "pressed" : "released");
                        if (window->__callback != NULL) {
                                window->__callback(window, key, pressed);
                        }
                        break;
                }
                case 'M':
                          window->__mouse_x = soc_recv_number(&window->__client);
                          window->__mouse_y = soc_recv_number(&window->__client);
                          //printf("server: mouse at %d:%d\n", window->__mouse_x, window->__mouse_y);
                          window->__waiting_for_mouse = false;
                          break;
                default:
                          printf("server: unkwonwn event\n");
                }
                window->__waiting_for_interrupt = false;
        }
        return NULL;
}

void gui_bootstrap(void)
{
        if (signal(SIGPIPE, sig_handler)) {
                fprintf(stderr, "gui_bootstrap: setting signal handler fail\n");
                abort();
        }
        g_server = soc_create_server(7777);
}

void gui_finalize(void)
{
        soc_close(g_server);
}

void gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
        window->__width = width;
        window->__height = height;
        window->__length = (unsigned long)width * height;

        window->__callback = NULL;
        window->__waiting_for_mouse = false;
        window->__waiting_for_interrupt = false;
        window->__mouse_x = 0;
        window->__mouse_y = 0;
        window->__key_reader_run = true;

        printf("server: waiting for client\n");
        soc_server_accept(g_server, &window->__client);

        window->__raw_pixels = malloc(window->__length * sizeof(unsigned));
        window->__compressed = malloc(window->__length * sizeof(unsigned));
        if (window->__raw_pixels == NULL || window->__compressed == NULL) {
                fprintf(stderr, "no memory\n");
                abort();
        }

        pthread_create(&window->__key_thread, NULL, key_reader_thread, window);

        soc_send_char(&window->__client, 'R');
        soc_send_number(&window->__client, width);
        soc_send_number(&window->__client, height);
}

void gui_destroy(struct gui_window *window)
{
        window->__key_reader_run = false;
        pthread_join(window->__key_thread, NULL);
}

unsigned int gui_width(struct gui_window *window)
{
        return window->__width;
}

unsigned int gui_height(struct gui_window *window)
{
        return window->__height;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
#ifdef DEBUG
        if (x >= window->__width || y >= window->__height) {
                fprintf(stderr, "gui_set_pixel(): out of bounds\n");
        }
#endif
        gui_set_pixel_raw(window, (unsigned long)y * window->__width + x, color);
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
        window->__raw_pixels[i] = color;
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
        if (x < window->__width && y < window->__height) {
                gui_set_pixel(window, x, y, color);
                return 0;
        } else {
                return EINVAL;
        }
}

unsigned *gui_raw_pixels(struct gui_window *window)
{
        return window->__raw_pixels;
}

void gui_draw(struct gui_window *window)
{
        unsigned long comp_size;
        char compress_amount;

	compress_amount = image_compress(window->__raw_pixels, window->__length,
					 window->__compressed, &comp_size);
	soc_send_char(&window->__client, 'B');
        soc_send_char(&window->__client, compress_amount);
        soc_send_number(&window->__client, comp_size);
        soc_send(&window->__client, window->__compressed, comp_size);
}

void gui_key_hook(struct gui_window *window, key_hook_t callback)
{
        window->__callback = callback;
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
        ((struct gui_window *)window)->__waiting_for_mouse = true;
        soc_send_char(&window->__client, 'm');
        while (window->__waiting_for_mouse) {
                usleep(10000);
        }

        *x = window->__mouse_x;
        *y = window->__mouse_y;
}

void gui_wfi(struct gui_window *window)
{
        window->__waiting_for_interrupt = true;
        while (window->__waiting_for_interrupt) {
                usleep(10000);
        }
}

