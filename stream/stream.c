
#include <guilib.h>

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#ifndef CONFIG_IMAGE_COMPRESS_TYPE
# define CONFIG_IMAGE_COMPRESS_TYPE 1
#endif

static int g_event_server;
static int g_pix_server;

void create_jpg(unsigned char **outbuffer, unsigned long *outlen, unsigned char *inbuffer, int width, int height, int quality);

static void image_encode(unsigned *image, unsigned long size,
			     unsigned char *dst, unsigned long *dst_size)
{
        for (unsigned long i = 0; i < size; i++) {
                dst[i * 3 + 2] = (image[i] & 0xFF0000) >> 16u;  // red
                dst[i * 3 + 1] = (image[i] & 0x00FF00) >> 8u;   // green
                dst[i * 3 + 0] = (image[i] & 0x0000FF);         // blue
        }

        *dst_size = size * 3;
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
                event = soc_recv_char(&window->event_socket);

                switch (event) {
                case 'K':
                case 'k': {
                        bool pressed = (event == 'K');
                        int key = soc_recv_number(&window->event_socket);
                        printf("server: key %d %s\n", key, pressed ? "pressed" : "released");
                        if (window->__callback != NULL) {
                                window->__callback(window, key, pressed);
                        }
                        break;
                }
                case 'M':
                          window->__mouse_x = soc_recv_number(&window->event_socket);
                          window->__mouse_y = soc_recv_number(&window->event_socket);
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
                //fprintf(stderr, "gui_bootstrap: setting signal handler fail\n");
                //abort();
        }
        g_event_server = soc_create_server(7777);
        g_pix_server = soc_create_server(7778);
}

void gui_finalize(void)
{
        soc_close(g_event_server);
        soc_close(g_pix_server);
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
        soc_server_accept(g_pix_server, &window->pix_socket);
        soc_server_accept(g_event_server, &window->event_socket);

        soc_send_char(&window->event_socket, 'R');
        soc_send_number(&window->event_socket, width);
        soc_send_number(&window->event_socket, height);
        if (CONFIG_IMAGE_COMPRESS_TYPE == 0) {
                soc_send_string(&window->event_socket, "EB0");
        } else if (CONFIG_IMAGE_COMPRESS_TYPE == 1) {
                soc_send_string(&window->event_socket, "EB1");
        }
        soc_send_flush(&window->event_socket);

        window->__raw_pixels = malloc(window->__length * sizeof(unsigned));

        pthread_create(&window->__key_thread, NULL, key_reader_thread, window);
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
        static unsigned char *temp = NULL;
        unsigned char *encoded = NULL;
        unsigned long encoded_size;

        if (temp == NULL) {
                temp = malloc(window->__length * 3);
        }
        image_encode(window->__raw_pixels, window->__length, temp, &encoded_size);

        if (CONFIG_IMAGE_COMPRESS_TYPE == 0) {
                encoded = temp;
        } else if (CONFIG_IMAGE_COMPRESS_TYPE == 1) {
	        create_jpg(&encoded, &encoded_size, temp, window->__width, window->__height, 50);
        }

        printf("encoded size %lu\n", encoded_size);
        soc_send_number(&window->pix_socket, encoded_size);
	soc_send(&window->pix_socket, encoded, encoded_size);
        soc_send_flush(&window->pix_socket);

        if (CONFIG_IMAGE_COMPRESS_TYPE == 1) {
                free(encoded);
        }
}

void gui_key_hook(struct gui_window *window, key_hook_t callback)
{
        window->__callback = callback;
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
        ((struct gui_window *)window)->__waiting_for_mouse = true;
        soc_send_char(&window->event_socket, 'm');
        soc_send_flush(&window->event_socket);
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

