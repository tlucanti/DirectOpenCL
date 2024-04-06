#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>

void create_jpg(unsigned char **outbuffer, unsigned long *outlen, unsigned char *inbuffer, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    /* Set up JPEG destination to memory. This will automatically malloc/realloc
     * *outbuffer to a sufficient size. After compression is finished, *outbuffer
     * will be the JPEG data and *outlen its size. */
    jpeg_mem_dest(&cinfo, outbuffer, outlen);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &inbuffer[cinfo.next_scanline * 3 * cinfo.image_width];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

