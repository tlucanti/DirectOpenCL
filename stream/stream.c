
#include <guilib.h>
#include <netsock.h>

#include <pthread.h>
#include <unistd.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPRESSION_TYPE CONFIG_GUILIB_STREAM_COMPRESSION_TYPE
#define QUALITY CONFIG_GUILIB_STREAM_COMPRESSION_QUALITY

struct gui_window {
	unsigned char pressed_keys[256];
	unsigned *raw_pixels;
	struct soc_stream event_socket;
	struct soc_stream pix_socket;
	pthread_t callback_thread;
	pthread_spinlock_t callback_lock;
	volatile int callback_key;
	volatile bool callback_pressed;
	volatile bool has_pending_callback;
	volatile bool callback_executor_run;
	unsigned int width;
	unsigned int height;
	unsigned long length;
	int mouse_x;
	int mouse_y;
	key_hook_t callback;
	pthread_t key_thread;
	volatile bool key_reader_run;
	volatile double last_iterrupt_time;
	volatile bool ready_to_start;
};

static int g_event_server;
static int g_pix_server;

int create_jpg(unsigned char **outbuffer, unsigned long *outlen,
	       unsigned char *inbuffer, int width, int height, int quality);

static double time_now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void image_encode(unsigned *image, unsigned long size,
			     unsigned char *dst, unsigned long *dst_size)
{
	for (unsigned long i = 0; i < size; i++) {
		dst[i * 3 + 0] = (image[i] & 0xFF0000) >> 16u;  // red
		dst[i * 3 + 1] = (image[i] & 0x00FF00) >> 8u;   // green
		dst[i * 3 + 2] = (image[i] & 0x0000FF);	 // blue
	}

	*dst_size = size * 3;
}

static void sig_handler(int sig)
{
	if (sig == SIGPIPE) {
		gui_panic("server killed by SIGPIPE");
	} else {
		fprintf(stderr, "got signal %d\n", sig);
		gui_panic("server killed by signal");
	}
}

static void *callback_executor_thread(void *winptr)
{
	struct gui_window *window = winptr;
	int key;
	bool pressed;

	while (window->callback_executor_run) {
		pthread_spin_lock(&window->callback_lock);
		if (window->has_pending_callback) {
			key = window->callback_key;
			pressed = window->callback_pressed;
			window->has_pending_callback = false;
			pthread_spin_unlock(&window->callback_lock);
			window->callback(window, key, pressed);
		} else {
			pthread_spin_unlock(&window->callback_lock);
			usleep(10000);
		}
	}

	return NULL;
}

static void *key_reader_thread(void *winptr)
{
	struct gui_window *window = winptr;
	char event;

	window->callback_executor_run = true;
	pthread_spin_init(&window->callback_lock, false);
	if (pthread_create(&window->callback_thread, NULL, callback_executor_thread, winptr)) {
		gui_panic("callback_executor thread spawn fail");
	}

	while (window->key_reader_run) {
		event = soc_recv_char(&window->event_socket);
		window->last_iterrupt_time = time_now();

		switch (event) {
		case 'S':
			WRITE_ONCE(window->ready_to_start, true);
			break;
		case 'K':
		case 'k': {
			bool pressed = (event == 'K');
			unsigned int key = soc_recv_number(&window->event_socket);

			if (key == GUI_CLOSED) {
				window->key_reader_run = false;
				printf("server: window closed\n");
			} else {
				printf("server: key %d %s\n", key, pressed ? "pressed" : "released");
			}
			window->pressed_keys[key] = pressed;

			if (window->callback != NULL) {
				while (true) {
					pthread_spin_lock(&window->callback_lock);
					if (window->has_pending_callback) {
						pthread_spin_unlock(&window->callback_lock);
						continue;
					}
					window->callback_key = key;
					window->callback_pressed = pressed;
					window->has_pending_callback = true;

					pthread_spin_unlock(&window->callback_lock);
					break;
				}
			}
			break;
		}
		case 'M':
			  window->mouse_x = soc_recv_number(&window->event_socket);
			  window->mouse_y = soc_recv_number(&window->event_socket);
			  printf("server: mouse at %d:%d\n", window->mouse_x, window->mouse_y);
			  break;
		default:
			  printf("server: unkwonwn event\n");
		}
	}

	window->callback_executor_run = false;
	pthread_join(window->callback_executor_run, NULL);
	return NULL;
}

int gui_bootstrap(void)
{
	if (signal(SIGPIPE, sig_handler)) {
		gui_perror("set signal handler fail");
	}

	g_event_server = soc_create_server(7777);
	if (g_event_server < 0) {
		gui_perror("create event socket fail");
		goto fail;
	}

	g_pix_server = soc_create_server(7778);
	if (g_pix_server < 0) {
		gui_perror("create image socket fail");
		goto fail;
	}

	return 0;

fail:
	soc_close(g_event_server);
	soc_close(g_pix_server);
	return EFAULT;
}

int gui_finalize(void)
{
	soc_close(g_event_server);
	soc_close(g_pix_server);
	return 0;
}

struct gui_window *gui_alloc(void)
{
	return malloc(sizeof(struct gui_window));
}

int gui_create(struct gui_window *window, unsigned int width, unsigned int height)
{
	int err;

	window->width = width;
	window->height = height;
	window->length = (unsigned long)width * height;

	window->callback = NULL;
	window->mouse_x = 0;
	window->mouse_y = 0;
	window->key_reader_run = true;
	memset(window->pressed_keys, 0, 256);

	window->raw_pixels = malloc(window->length * sizeof(unsigned));
	if (window->raw_pixels == NULL) {
		gui_perror("out of memory");
		err = ENOMEM;
		goto fail;
	}

	printf("server: waiting for client\n");
	soc_server_accept(g_pix_server, &window->pix_socket);
	soc_server_accept(g_event_server, &window->event_socket);

	window->ready_to_start = false;
	if (pthread_create(&window->key_thread, NULL, key_reader_thread, window)) {
		gui_perror("key_reader thread spawn fail");
		err = EFAULT;
		goto fail;
	}

	soc_send_char(&window->event_socket, 'R');
	soc_send_number(&window->event_socket, width);
	soc_send_number(&window->event_socket, height);
	if (COMPRESSION_TYPE == 0) {
		soc_send_string(&window->event_socket, "EB0");
	} else if (COMPRESSION_TYPE == 1) {
		soc_send_string(&window->event_socket, "EB1");
	} else {
		gui_panic("invalid image compress type");
	}
	soc_send_flush(&window->event_socket);

	while (READ_ONCE(window->ready_to_start) == 0) {}

	return 0;

fail:
	gui_destroy(window);
	return err;
}

int gui_destroy(struct gui_window *window)
{
	free(window->raw_pixels);

	window->key_reader_run = false;
	if (pthread_join(window->key_thread, NULL)) {
		gui_perror("key_reader join fail");
		return EFAULT;
	}
	return 0;
}

unsigned int gui_width(struct gui_window *window)
{
	return window->width;
}

unsigned int gui_height(struct gui_window *window)
{
	return window->height;
}

void gui_set_pixel(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
#ifdef CONFIG_GUILIB_DEBUG
	if (x >= window->width || y >= window->height) {
		fprintf(stderr, "gui_set_pixel(): out of bounds: x %u/%u, y %u/%u\n",
			x, gui_width(window), y, gui_height(window));
	}
#endif
	gui_set_pixel_raw(window, (unsigned long)y * window->width + x, color);
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	window->raw_pixels[i] = color;
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	if (x < window->width && y < window->height) {
		gui_set_pixel(window, x, y, color);
		return 0;
	} else {
		return EINVAL;
	}
}

unsigned *gui_raw_pixels(struct gui_window *window)
{
	return window->raw_pixels;
}

int gui_draw(struct gui_window *window)
{
	static unsigned char *temp = NULL;
	unsigned char *encoded = NULL;
	unsigned long encoded_size;

	if (temp == NULL) {
		temp = malloc(window->length * 3);
		if (temp == NULL) {
			gui_perror("out of memory");
			return ENOMEM;
		}
	}
	image_encode(window->raw_pixels, window->length, temp, &encoded_size);

	if (COMPRESSION_TYPE == 0) {
		encoded = temp;
	} else if (COMPRESSION_TYPE == 1) {
		if (create_jpg(&encoded, &encoded_size, temp, window->width,
			       window->height, QUALITY)) {
			gui_perror("jpeg compression fail");
			free(encoded);
			return EFAULT;
		}
	}

	// printf("encoded size %lu\n", encoded_size);
	soc_send_number(&window->pix_socket, encoded_size);
	soc_send(&window->pix_socket, encoded, encoded_size);
	soc_send_flush(&window->pix_socket);

	if (COMPRESSION_TYPE == 1) {
		free(encoded);
	}
	return 0;
}

void gui_key_hook(struct gui_window *window, key_hook_t callback)
{
	window->callback = callback;
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
	*x = window->mouse_x;
	*y = window->mouse_y;
}

void gui_wfi(struct gui_window *window)
{
	double now = time_now();
	while (window->last_iterrupt_time < now) {
		usleep(10000);
	}
}

bool gui_closed(struct gui_window *window)
{
	return gui_pressed(window, GUI_CLOSED);
}

bool gui_pressed(struct gui_window *window, unsigned char keycode)
{
	return window->pressed_keys[keycode];
}

