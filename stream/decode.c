
#include <stddef.h>
#include <stdio.h>
#include "jpeg-8d/jpeglib.h"
#include <stdlib.h>

int main()
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	unsigned char binary_array[] = {
		255, 216, 255, 224, 0,	 16,  74,  70,	73,  70,  0,   1,   1,
		0,   0,	  1,   0,   1,	 0,   0,   255, 219, 0,	  67,  0,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   255, 219,
		0,   67,  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   1,   1,	1,   1,	  1,   1,   1,
		1,   1,	  255, 192, 0,	 17,  8,   0,	2,   0,	  2,   3,   1,
		34,  0,	  2,   17,  1,	 3,   17,  1,	255, 196, 0,   31,  0,
		0,   1,	  5,   1,   1,	 1,   1,   1,	1,   0,	  0,   0,   0,
		0,   0,	  0,   0,   1,	 2,   3,   4,	5,   6,	  7,   8,   9,
		10,  11,  255, 196, 0,	 181, 16,  0,	2,   1,	  3,   3,   2,
		4,   3,	  5,   5,   4,	 4,   0,   0,	1,   125, 1,   2,   3,
		0,   4,	  17,  5,   18,	 33,  49,  65,	6,   19,  81,  97,  7,
		34,  113, 20,  50,  129, 145, 161, 8,	35,  66,  177, 193, 21,
		82,  209, 240, 36,  51,	 98,  114, 130, 9,   10,  22,  23,  24,
		25,  26,  37,  38,  39,	 40,  41,  42,	52,  53,  54,  55,  56,
		57,  58,  67,  68,  69,	 70,  71,  72,	73,  74,  83,  84,  85,
		86,  87,  88,  89,  90,	 99,  100, 101, 102, 103, 104, 105, 106,
		115, 116, 117, 118, 119, 120, 121, 122, 131, 132, 133, 134, 135,
		136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162,
		163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182,
		183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202,
		210, 211, 212, 213, 214, 215, 216, 217, 218, 225, 226, 227, 228,
		229, 230, 231, 232, 233, 234, 241, 242, 243, 244, 245, 246, 247,
		248, 249, 250, 255, 196, 0,   31,  1,	0,   3,	  1,   1,   1,
		1,   1,	  1,   1,   1,	 1,   0,   0,	0,   0,	  0,   0,   1,
		2,   3,	  4,   5,   6,	 7,   8,   9,	10,  11,  255, 196, 0,
		181, 17,  0,   2,   1,	 2,   4,   4,	3,   4,	  7,   5,   4,
		4,   0,	  1,   2,   119, 0,   1,   2,	3,   17,  4,   5,   33,
		49,  6,	  18,  65,  81,	 7,   97,  113, 19,  34,  50,  129, 8,
		20,  66,  145, 161, 177, 193, 9,   35,	51,  82,  240, 21,  98,
		114, 209, 10,  22,  36,	 52,  225, 37,	241, 23,  24,  25,  26,
		38,  39,  40,  41,  42,	 53,  54,  55,	56,  57,  58,  67,  68,
		69,  70,  71,  72,  73,	 74,  83,  84,	85,  86,  87,  88,  89,
		90,  99,  100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118,
		119, 120, 121, 122, 130, 131, 132, 133, 134, 135, 136, 137, 138,
		146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165,
		166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185,
		186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212,
		213, 214, 215, 216, 217, 218, 226, 227, 228, 229, 230, 231, 232,
		233, 234, 242, 243, 244, 245, 246, 247, 248, 249, 250, 255, 218,
		0,   12,  3,   1,   0,	 2,   17,  3,	17,  0,	  63,  0,   247,
		15,  28,  124, 44,  248, 99,  225, 143, 26,  120, 191, 195, 126,
		27,  248, 113, 224, 63,	 15,  120, 119, 195, 222, 40,  241, 6,
		135, 160, 104, 26,  31,	 132, 60,  61,	164, 232, 186, 30,  139,
		164, 234, 215, 118, 26,	 86,  143, 163, 233, 86,  26,  117, 189,
		142, 153, 165, 233, 150, 54,  240, 89,	105, 250, 125, 148, 16,
		90,  89,  218, 65,  13,	 181, 180, 49,	195, 26,  34,  148, 81,
		95,  144, 226, 63,  222, 43,  127, 215, 234, 159, 250, 92,  79,
		243, 247, 56,  255, 0,	 145, 198, 105, 255, 0,	  99,  44,  111,
		254, 165, 51,  255, 217
	};
	unsigned long binary_array_length = sizeof(binary_array);

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, binary_array, binary_array_length);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned char *row_pointer = (unsigned char *)malloc(
		cinfo.output_width * cinfo.num_components);
	unsigned long decoded_size =
		cinfo.output_width * cinfo.output_height * cinfo.num_components;
	unsigned char *decoded = malloc(decoded_size);
	unsigned loc = 0;

	while (cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, &row_pointer, 1);
		for (unsigned i = 0;
		     i < cinfo.image_width * cinfo.num_components; i++)
			decoded[loc++] = row_pointer[i];
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer);

	for (unsigned long i = 0; i < decoded_size; i++) {
                printf("%d ", decoded[i]);
        }
        printf("\n");
}

