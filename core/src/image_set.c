#include <stdlib.h>

#include "image_set.h"


ImageSet* ImageSet_construct(int length, int channels, int height, int width, int output_length) {
	ImageSet* image_set = (ImageSet*)malloc(sizeof(ImageSet));

	image_set->__current_length__ = 0;
	image_set->length = length;
	image_set->channels = channels;
	image_set->height = height;
	image_set->width = width;
	image_set->input = (double****)malloc(sizeof(double***) * image_set->length);
	for (int i = 0; i < image_set->length; ++i) {
		image_set->input[i] = (double***)malloc(sizeof(double**) * image_set->channels);
		for (int c = 0; c < image_set->channels; ++c) {
			image_set->input[i][c] = (double**)malloc(sizeof(double*) * image_set->height);
			for (int h = 0; h < image_set->height; ++h) {
				image_set->input[i][c][h] = (double*)malloc(sizeof(double) * image_set->width);
			}
		}
	}

	image_set->output_length = output_length;
	image_set->output = (double**)malloc(sizeof(double*) * image_set->length);
	for (int i = 0; i < image_set->length; ++i) {
		image_set->output[i] = (double*)malloc(sizeof(double) * image_set->output_length);
	}
	
	return image_set;
}

void ImageSet_destruct(ImageSet* image_set) {
	for (int i = 0; i < image_set->length; ++i) {
		for (int c = 0; c < image_set->channels; ++c) {
			for (int h = 0; h < image_set->height; ++h) {
				free(image_set->input[i][c][h]);
			}
			free(image_set->input[i][c]);
		}
		free(image_set->input[i]);
	}
	free(image_set->input);

	for (int i = 0; i < image_set->length; ++i) {
		free(image_set->output[i]);
	}
	free(image_set->output);
	
	free(image_set);
}

void ImageSet_add(ImageSet* image_set, double input[], double output[]) {
	if (image_set->__current_length__ == image_set->length) {
		++image_set->length;
		++image_set->__current_length__;

		image_set->input = (double****)realloc(image_set->input, sizeof(double***) * image_set->length);
		image_set->input[image_set->length - 1] = (double***)malloc(sizeof(double**) * image_set->channels);
		for (int c = 0; c < image_set->channels; ++c) {
			image_set->input[image_set->length - 1][c] = (double**)malloc(sizeof(double*) * image_set->height);
			for (int h = 0; h < image_set->height; ++h) {
				image_set->input[image_set->length - 1][c][h] = (double*)malloc(sizeof(double) * image_set->width);
			}
		}

		image_set->output = (double**)realloc(image_set->output, sizeof(double*) * image_set->length);
		image_set->output[image_set->length - 1] = (double*)malloc(sizeof(double) * image_set->output_length);
	} else {
		++image_set->__current_length__;
	}

	for (int c = 0; c < image_set->channels; ++c) {
		for (int h = 0; h < image_set->height; ++h) {
			for (int w = 0; w < image_set->width; ++w) {
				image_set->input[image_set->__current_length__ - 1][c][h][w] = input[c * image_set->height * image_set->width + h * image_set->width + w];
			}
		}
	}

	for (int i = 0; i < image_set->output_length; ++i) {
		image_set->output[image_set->__current_length__ - 1][i] = output[i];
	}
}