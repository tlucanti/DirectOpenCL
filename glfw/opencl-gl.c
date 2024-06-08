
#define CL_TARGET_OPENCL_VERSION 200
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glx.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <guilib.h>

struct gui_window {
	GLFWwindow *glfw_window;
	cl_platform_id cl_platform;
	cl_device_id cl_device;
	cl_context cl_context;
	cl_command_queue cl_queue;
	GLuint gl_texture;
	unsigned width;
	unsigned height;
};

int gui_bootstrap(void)
{
	if (glfwInit() != GLFW_TRUE)
		gui_panic("failed to initialize glfw");
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

static void glfw_init(struct gui_window *window)
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window->glfw_window = glfwCreateWindow(window->width, window->height, "opencl", NULL, NULL);
	if (window->glfw_window == NULL)
		gui_panic("failed to create glfw window");
	glfwMakeContextCurrent(window->glfw_window);
}

static void opencl_init(struct gui_window *window)
{
	cl_uint numPlatforms = 0;
	cl_uint numDevices = 0;
	cl_int err;

	/* platform */
	if (clGetPlatformIDs(0, NULL, &numPlatforms))
		gui_panic("failed to query cl platforms");
	if (numPlatforms == 0)
		gui_panic("no platforms avaliable");
	else
		printf("avaliable %u platforms, selecting first\n", numPlatforms);
	if (clGetPlatformIDs(1, &window->cl_platform, NULL))
		gui_panic("failed to query cl platforms");

	/* device */
	if (clGetDeviceIDs(window->cl_platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices))
		gui_panic("failed to query cl devices");
	if (numDevices == 0)
		gui_panic("no devices avaliable");
	else
		printf("avaliable %u devices, selecting first\n", numDevices);
	if (clGetDeviceIDs(window->cl_platform, CL_DEVICE_TYPE_ALL, 1, &window->cl_device, NULL))
		gui_panic("failed to query cl devices");

	/* context */
	cl_context_properties props[7];
	props[0] = CL_GL_CONTEXT_KHR;
	props[1] = (cl_context_properties)glfwGetGLXContext(window->glfw_window);
	if (glGetError())
		gui_panic("failed to get cl context properties");

	props[2] = CL_GLX_DISPLAY_KHR;
	props[3] = (cl_context_properties)glfwGetX11Display();
	if (glGetError())
		gui_panic("failed to get cl context properties");

	props[4] = CL_CONTEXT_PLATFORM;
	props[5] = (cl_context_properties)(window->cl_platform);
	if (glGetError())
		gui_panic("failed to get cl context properties");

	props[6] = 0;
	window->cl_context = clCreateContext(props, 1, &window->cl_device, NULL, NULL, &err);
	if (err) {
		printf("%d\n", err);
		gui_panic("failed to create cl context from gl context");
	}

	/* queue */
	window->cl_queue = clCreateCommandQueueWithProperties(window->cl_context, window->cl_device, NULL, &err);
	if (err)
		gui_panic("failed to create cl queue");
}

static void opengl_init(struct gui_window *window)
{
	cl_int err;

	glGenTextures(1, &window->gl_texture);
	if (glGetError())
		gui_panic("failed to create gl texture");
	glBindTexture(GL_TEXTURE_2D, window->gl_texture);
	if (glGetError())
		gui_panic("failed to bind gl texture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->width, window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (glGetError())
		gui_panic("failed to define texture image");
	clCreateFromGLTexture(window->cl_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, window->gl_texture, &err);
	if (err)
		gui_panic("failed to create cl image from gl texture");
}

int gui_create(struct gui_window *window, unsigned width, unsigned height)
{
	window->width = width;
	window->height = height;
	glfw_init(window);
	opencl_init(window);
	opengl_init(window);

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
	abort();
}

void gui_set_pixel_raw(struct gui_window *window, unsigned long i, unsigned color)
{
	abort();
}

int gui_set_pixel_safe(struct gui_window *window, unsigned x, unsigned y, unsigned color)
{
	abort();
}

unsigned *gui_raw_pixels(struct gui_window *window)
{
	abort();
}

int gui_draw(struct gui_window *window)
{
	abort();
}

void gui_key_hook(struct gui_window *window, key_hook_t hook)
{
	abort();
}

void gui_mouse(struct gui_window *window, int *x, int *y)
{
	abort();
}

void gui_wfi(struct gui_window *window)
{
	abort();
}

bool gui_pressed(struct gui_window *window, unsigned char key)
{
	abort();
}

bool gui_closed(struct gui_window *window)
{
	return glfwWindowShouldClose(window->glfw_window);
}

int main()
{
	struct gui_window *window = gui_alloc();
	gui_bootstrap();
	gui_create(window, 800, 600);
	gui_destroy(window);
	gui_finalize();
}
