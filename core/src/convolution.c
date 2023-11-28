#include <stdlib.h>

#include "library.h"
#include "convolution.h"


#define activation(x) ReLU_f(x)


Convolution* Convolution_construct(int channels, int height, int width, int matrix_c, int matrix_h, int matrix_w, int padding, int stride_h, int stride_w) {
	Convolution* layer = (Convolution*)malloc(Convolution);

	layer->channels = channels;
	layer->height = height;
	layer->width = width;

	layer->matrix_c = matrix_c;
	layer->matrix_h = matrix_h;
	layer->matrix_w = matrix_w;
	
	layer->padding = padding;
	
	layer->stride_h = stride_h;
	layer->stride_w = stride_w;

	layer->input = (double***)malloc(sizeof(double**) * layer->channels);
    for (int c = 0; c < layer->channels; ++c) {
        layer->input[c] = (double**)malloc(sizeof(double*) * (layer->height + 2 * layer->padding));
        for (int hp = 0; hp < layer->height + 2 * layer->padding; ++hp) {
            layer->input[c][hp] = (double*)malloc(sizeof(double) * (layer->width + 2 * layer->padding));
        }
    }

    layer->output = (double***)malloc(sizeof(double**) * layer->channels);
    for (int c = 0; c < layer->channels; ++c) {
        layer->output[c] = (double**)malloc(sizeof(double*) * (layer->height + 2 * layer->padding) / layer->matrix_h);
        for (int oh = 0; oh < layer->height / layer->matrix_h; ++oh) {
            layer->output[c][oh] = (double*)malloc(sizeof(double) * (layer->width + 2 * layer->padding) / layer->matrix_w);
        }
    }

    layer->matrix = (double****)malloc(sizeof(double***) * layer->matrix_c);
    for (int mc = 0; mc < layer->matrix_c; ++mc) {
	    layer->matrix[mc] = (double***)malloc(sizeof(double**) * layer->channels);
	    for (int c = 0; c < layer->channels; ++c) {
	    	layer->matrix[mc][c] = (double**)malloc(sizeof(double*) * layer->matrix_h);
		    for (int mh = 0; mh < layer->matrix_h; ++mh) {
			    layer->matrix[mc][c][mh] = (double*)malloc(sizeof(double) * layer->matrix_w);
			    for (int mw = 0; mw < layer->matrix_w; ++mw) {
			    	layer->matrix[mc][c][mh][mw] = 1; // random(0, 1);
			    }
		    }
		}
	}

	layer->bias = (double***)malloc(sizeof(double**) * layer->matrix_c);
	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		layer->bias[mc] = (double**)malloc(sizeof(double*) * (1 + (layer->height + 2 * layer->padding - layer->matrix_h) / layer->stride_h));
		for (int hh = 0; hh < 1 + (layer->height + 2 * layer->padding - layer->matrix_h) / layer->stride_h; ++hh) {
			layer->bias[mc][hh] = (double*)malloc(sizeof(double) * (1 + (layer->width + 2 * layer->padding - layer->matrix_w) / layer->stride_w));
			for (int ww = 0; ww < 1 + (layer->width + 2 * layer->padding - layer->matrix_w) / layer->stride_w; ++ww) {
				layer->bias[mc][hh][ww] = 1; // random(0, 1);
			}
		}
	}

	return layer;
}

void Convolution_destruct(Convolution* layer) {
    for (int c = 0; c < layer->channels; ++c) {
        for (int hp = 0; hp < layer->height + 2 * layer->padding; ++hp) {
            free(layer->input[c][hp]);
        }
        free(layer->input[c]);
    }
    free(layer->input);

    for (int c = 0; c < layer->channels; ++c) {
        for (int oh = 0; oh < (layer->height + 2 * layer->padding) / layer->matrix_h; ++oh) {
            free(layer->output[c][oh]);
        }
        free(layer->output[c]);
    }
    free(layer->output);
    
    for (int mc = 0; mc < layer->matrix_c; ++mc) {
	    for (int c = 0; c < layer->channels; ++c) {
		    for (int mh = 0; mh < layer->matrix_h; ++mh) {
		    	free(layer->matrix[mc][c][mh]);
		    }
		    free(layer->matrix[mc][c]);
		}
		free(layer->matrix[mc]);
	}
	free(layer->matrix);

	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int hh = 0; hh < 1 + (layer->height + 2 * layer->padding - layer->matrix_h) / layer->stride_h; ++hh) {
			free(layer->bias[mc][hh]);
		}
		free(layer->bias[mc]);
	}
	free(layer->bias);

    free(layer);
}

void Convolution_train_construct(Convolution* layer, double learning_rate, double momentum);
void Convolution_train_destruct(Convolution* layer);

void Convolution_forward(Convolution* layer, double*** input) {
	// copying input to the layer->input IS NOT EFFICIENT,
	// you can just change pointer layer->input to the input,
	// if you will not change layer->input data and you will not change data in input (ImageSet)
	// you also can even dont save input data and get it only in functions
	// I see problems with converting list[1 * n] to list[channels * height * width]
	// When output from FullConnected layer come to the Convolution input
	for (int c = 0; c < layer->channels; ++c) {
		for (int h = 0; h < layer->height; ++h) {
			for (int w = 0; w < layer->width; ++w) {
				layer->input[c][layer->padding + h][layer->padding + w] = input[c][h][w];
			}
		}
	}

	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int hh = 0; hh < 1 + (layer->height + 2 * layer->padding - layer->matrix_h) / layer->stride_h; ++hh) {
			for (int ww = 0; ww < 1 + (layer->width + 2 * layer->padding - layer->matrix_w) / layer->stride_w; ++ww) {
				layer->output[mc][hh][ww] = layer->bias[mc][hh][ww];
				for (int c = 0; c < layer->channels; ++c) {
					for (int mh = 0; mh < layer->matrix_h; ++mh) {
						for (int mw = 0; mw < layer->matrix_w; ++mw) {
							layer->output[mc][hh][ww] += layer->input[c][hh * layer->stride_h + mh][ww * layer->stride_w + mw] * layer->matrix[mc][c][mh][mw];
						}
					}
				}
				layer->output[mc][hh][ww] = activation(layer->output[mc][hh][ww]);
			}
		}
	}

    // for mc in range(self.matrix_c):
    //     for hh in range(1 + (self.height - self.matrix_h) / self.stride_h):
    //         for ww in range(1 + (self.width - self.matrix_w) / self.stride_w):
    //             self.x[mc][hh][ww] = self.bias[mc][hh][ww]
    //             for c in range(self.channels):
    //                 for mh in range(self.matrix_h):
    //                     for mw in range(self.matrix_w):
    //                         self.x[mc][hh][ww] += self.data_input[c][hh * self.stride_h + mh][ww * self.stride_w + mw] * self.w[mc][c][mh][mw]
    //             self.x[mc][hh][ww] = self.activation.f(self.x[mc][hh][ww])
}

void Convolution_backward(Convolution* layer, double*** output);