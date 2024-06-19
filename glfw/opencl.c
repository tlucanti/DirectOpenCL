
#define CL_TARGET_OPENCL_VERSION 200

#include <GLFW/glfw3.h>
#include <CL/cl.h>
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
	cl_program cl_program;
	cl_kernel cl_kernel;
	unsigned width;
	unsigned height;
};

static char *readFile(const char *fname)
{
	FILE *f;
	long size;
	char *buff;

	f = fopen(fname, "rb");
	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	size = ftell(f);

	buff = malloc(size + 1);
	if (buff == NULL)
		return NULL;
	fread(buff, size, 1, f);
	fclose(f);
	buff[size] = 0;

	return buff;
}

int gui_bootstrap(void)
{
	if (glfwInit() != GLFW_TRUE)
		gui_panic("failed to init glfw");
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
	window->glfw_window = glfwCreateWindow(window->width, window->height, "opencl", NULL, NULL);
	if (window->glfw_window == NULL)
		gui_panic("failed to create glfw window");
	glfwMakeContextCurrent(window->glfw_window);
}

static void opencl_init(struct gui_window *window, const char *kernel_file)
{
	char *fileContents;
	cl_uint numPlatforms = 0;
	cl_uint numDevices = 0;
	size_t logSize;
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
	window->cl_context = clCreateContext(NULL, 1, &window->cl_device, NULL, NULL, &err);
	if (err)
		gui_panic("failed to create cl context");

	/* queue */
	window->cl_queue = clCreateCommandQueueWithProperties(window->cl_context, window->cl_device, NULL, &err);
	if (err)
		gui_panic("failed to create cl queue");

	/* program */
	fileContents = readFile(kernel_file);
	if (fileContents == NULL)
		gui_panic("failed to read kernel source file");
	window->cl_program = clCreateProgramWithSource(window->cl_context, 1, (const char **)&fileContents, NULL, &err);
	if (err == CL_BUILD_PROGRAM_FAILURE) {
		printf("kernel compilation error\n");
		err = clGetProgramBuildInfo(window->cl_program, window->cl_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		if (err)
			gui_panic("failed to get program build info");

		char log[logSize];
		err = clGetProgramBuildInfo(window->cl_program, window->cl_device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);
		if (err)
			gui_panic("failed to get program build log");
		printf("%s\n", log);
		gui_panic("compilation error");
	} else if (err)
		gui_panic("failed to build cl kernel");

	/* kerel */
	window->cl_kernel = clCreateKernel(window->cl_program, "main", &err);
	if (err)
		gui_panic("failed to create kernel");

	/* image */
	window->cl_image = clCreate();

	free(fileContents);
}

int gui_create_cl(struct gui_window *window, unsigned width, unsigned height, const char *kernel_file)
{
	window->width = width;
	window->height = height;
	glfw_init(window);
	opencl_init(window, kernel_file);

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

int gui_draw_cl(struct gui_window *window, void *kernel_arg)
{
	clSetKernelArg(window->cl_kernel, 0, sizeof());
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
