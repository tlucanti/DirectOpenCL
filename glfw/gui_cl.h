
struct gui_image {
	__global unsigned *buffer;
	unsigned x;
	unsigned y;
};

extern main(struct gui_image *image, unsigned x, unsigned int y, void *user_arg);

static inline void gui_set_pixel(struct gui_image *image, unsigned x, unsigned y, unsigned color)
{
	gui_set_pixel_raw(image);
}

static inline void gui_set_pixel_raw(struct gui_image *image, unsigned long i, unsigned color)
{
}

static inline void gui_set_pixel_safe(struct gui_image *image, unsigned x, unsigned y, unsigned color)
{
}

static inline unsigned gui_get_pixel(struct gui_image *image, unsigned long x, unsigned long y)
{
}

static inline unsigned gui_get_pixel_raw(struct gui_image *image, unsigned long i)
{
}

static inline unsigned gui_get_pixel_safe(struct gui_image *image, unsigned x, unsigned y)
{
}

__kernel void __gui_kernel(__global unsigned *buffer, unsigned width, unsigned height, void *user_arg)
{
	struct gui_image image = {
		.buffer = buffer,
		.width = width,
		.height = heiht,
	};

	main(&image, get_global_id(0), get_global_id(1), user_arg)
}

