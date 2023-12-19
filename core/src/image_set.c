#include <stdio.h>
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

void ImageSet_save(ImageSet* image_set) {
    FILE* file = fopen("data.txt", "w");
    if (file == NULL) {
        printf("!!! ERROR in file handling for saving !!!\n");
    }
    fprintf(file, "%d %d %d %d %d ", image_set->length, image_set->channels, image_set->height, image_set->width, image_set->output_length);

    for (int i = 0; i < image_set->length; ++i) {
    	for (int c = 0; c < image_set->channels; ++c) {
    		for (int h = 0; h < image_set->height; ++h) {
    			for (int w = 0; w < image_set->width; ++w) {
    				fprintf(file, "%lf ", image_set->input[i][c][h][w]);
    			}
    		}
    	}
    	for (int o = 0; o < image_set->output_length; ++o) {
    		fprintf(file, "%lf ", image_set->output[i][o]);
    	}
    }
    
    fclose(file);
}

void ImageSet_load(ImageSet* image_set) {
    FILE* file = fopen("data.txt", "r");
    if (file == NULL) {
        printf("!!! ERROR in file handling for saving !!!\n");
    }

    int length, channels, height, width, output_length;
    fscanf(file, "%d %d %d %d %d ", &length, &channels, &height, &width, &output_length);
    if (channels != image_set->channels) {
    	printf("!!! ERROR loaded ImageSet channels (%d) dont equal current ImageSet (%d) !!!\n", channels, image_set->channels);
    }
    if (height != image_set->height) {
    	printf("!!! ERROR loaded ImageSet height (%d) dont equal current ImageSet (%d) !!!\n", height, image_set->height);
    }
    if (width != image_set->width) {
    	printf("!!! ERROR loaded ImageSet width (%d) dont equal current ImageSet (%d) !!!\n", width, image_set->width);
    }
    if (output_length != image_set->output_length) {
    	printf("!!! ERROR loaded ImageSet output_length (%d) dont equal current ImageSet (%d) !!!\n", output_length, image_set->output_length);
    }

    for (int i = 0; i < image_set->length; ++i) {
    	for (int c = 0; c < image_set->channels; ++c) {
    		for (int h = 0; h < image_set->height; ++h) {
    			for (int w = 0; w < image_set->width; ++w) {
    				fscanf(file, "%lf ", &image_set->input[i][c][h][w]);
    			}
    		}
    	}
    	for (int o = 0; o < image_set->output_length; ++o) {
    		fscanf(file, "%lf ", &image_set->output[i][o]);
    	}
    }
    
    fclose(file);
}