
#include <GLFW/glfw3.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <guilib.h>

struct gui_window {
	GLFWwindow *glfw_window;
	unsigned *raw_pixels;
	bool pressed[256];
	unsigned width;
	unsigned height;
};

int gui_bootstrap(void)
{
	if (glfwInit() != GLFW_TRUE) {
		return EFAULT;
	}

	return 0;
}

int gui_finalize(void)
{
	glfwTerminate();
	return 0;
}

struct gui_window *gui_alloc(void)
{
	return malloc(sizeof(struct gui_window));
}

int gui_create(struct gui_window *window, unsigned width, unsigned height)
{
	window->raw_pixels = malloc(sizeof(unsigned) * width * height);
	if (window->raw_pixels == NULL) {
		return ENOMEM;
	}

	window->glfw_window = glfwCreateWindow(width, height, "guilib", NULL, NULL);
	if (window->glfw_window == NULL) {
	        return EFAULT;
	}

	glfwMakeContextCurrent(window->glfw_window);

	window->width = width;
	window->height = height;

	memset(window->pressed, 0, 256);

	return 0;
}

int gui_destroy(struct gui_window *window)
{
	glfwDestroyWindow(window->glfw_window);
	return 0;
}

unsigned gui_width(struct gui_window *window)
{
	return window->width;
}

unsigned gui_height(struct gui_window *window)
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
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(window->width, window->height, GL_RGBA, GL_UNSIGNED_BYTE, window->raw_pixels);
	glfwSwapBuffers(window->glfw_window);
	glfwPollEvents();

	return 0;
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	// gui_panic("not implemented");
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
	double xpos, ypos;
	glfwGetCursorPos(window->glfw_window, &xpos, &ypos);

	*x = xpos;
	*y = ypos;
}

void gui_wfi(struct gui_window *window)
{
	// gui_panic("not implemented");
}

bool gui_pressed(struct gui_window *window, unsigned char key)
{
	// gui_panic("not implemented");
}

bool gui_closed(struct gui_window *window)
{
	return glfwWindowShouldClose(window->glfw_window);
}

