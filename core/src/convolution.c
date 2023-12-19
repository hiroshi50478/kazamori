#include <stdlib.h>

#include "library.h"
#include "convolution.h"


#define activation(x) Sigmoid_f(x)


Convolution* Convolution_construct(int channels, int height, int width, int matrix_c, int matrix_h, int matrix_w, int padding, int stride_h, int stride_w) {
	Convolution* layer = (Convolution*)malloc(sizeof(Convolution));

	layer->channels = channels;
	layer->height = height;
	layer->width = width;

	layer->matrix_c = matrix_c;
	layer->matrix_h = matrix_h;
	layer->matrix_w = matrix_w;
	
	layer->padding = padding;
	
	layer->stride_h = stride_h;
	layer->stride_w = stride_w;

	layer->__input_height = layer->height + 2 * layer->padding;
	layer->__input_width = layer->width + 2 * layer->padding;
	layer->__output_height = 1 + (layer->height + 2 * layer->padding - layer->matrix_h) / layer->stride_h;
	layer->__output_width = 1 + (layer->width + 2 * layer->padding - layer->matrix_w) / layer->stride_w;

	layer->input = (double***)malloc(sizeof(double**) * layer->channels);
    for (int c = 0; c < layer->channels; ++c) {
        layer->input[c] = (double**)malloc(sizeof(double*) * layer->__input_height);
        for (int ih = 0; ih < layer->__input_height; ++ih) {
            layer->input[c][ih] = (double*)malloc(sizeof(double) * layer->__input_width);
        }
    }

    layer->output = (double***)malloc(sizeof(double**) * layer->matrix_c);
    for (int mc = 0; mc < layer->matrix_c; ++mc) {
        layer->output[mc] = (double**)malloc(sizeof(double*) * layer->__output_height);
        for (int oh = 0; oh < layer->__output_height; ++oh) {
            layer->output[mc][oh] = (double*)malloc(sizeof(double) * layer->__output_width);
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
		layer->bias[mc] = (double**)malloc(sizeof(double*) * layer->__output_height);
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			layer->bias[mc][oh] = (double*)malloc(sizeof(double) * layer->__output_width);
			for (int ow = 0; ow < layer->__output_width; ++ow) {
				layer->bias[mc][oh][ow] = 1; // random(0, 1);
			}
		}
	}

	return layer;
}

void Convolution_destruct(Convolution* layer) {
    for (int c = 0; c < layer->channels; ++c) {
        for (int ih = 0; ih < layer->__input_height; ++ih) {
            free(layer->input[c][ih]);
        }
        free(layer->input[c]);
    }
    free(layer->input);

    for (int mc = 0; mc < layer->matrix_c; ++mc) {
        for (int oh = 0; oh < layer->__output_height; ++oh) {
            free(layer->output[mc][oh]);
        }
        free(layer->output[mc]);
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
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			free(layer->bias[mc][oh]);
		}
		free(layer->bias[mc]);
	}
	free(layer->bias);

    free(layer);
}

void Convolution_train_construct(Convolution* layer, double learning_rate, double momentum) {
	layer->learning_rate = learning_rate;
	layer->momentum = momentum;

	layer->err = (double***)malloc(sizeof(double**) * layer->channels);
	for (int c = 0; c < layer->channels; ++c) {
		layer->err[c] = (double**)malloc(sizeof(double*) * layer->height);
		for (int h = 0; h < layer->height; ++h) {
			layer->err[c][h] = (double*)malloc(sizeof(double) * layer->width);
		}
	}

	layer->delta_matrix = (double****)malloc(sizeof(double***) * layer->matrix_c);
    for (int mc = 0; mc < layer->matrix_c; ++mc) {
	    layer->delta_matrix[mc] = (double***)malloc(sizeof(double**) * layer->channels);
	    for (int c = 0; c < layer->channels; ++c) {
	    	layer->delta_matrix[mc][c] = (double**)malloc(sizeof(double*) * layer->matrix_h);
		    for (int mh = 0; mh < layer->matrix_h; ++mh) {
			    layer->delta_matrix[mc][c][mh] = (double*)malloc(sizeof(double) * layer->matrix_w);
			    for (int mw = 0; mw < layer->matrix_w; ++mw) {
			    	layer->delta_matrix[mc][c][mh][mw] = 0;
			    }
		    }
		}
	}

	layer->delta_bias = (double***)malloc(sizeof(double**) * layer->matrix_c);
	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		layer->delta_bias[mc] = (double**)malloc(sizeof(double*) * layer->__output_height);
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			layer->delta_bias[mc][oh] = (double*)malloc(sizeof(double) * layer->__output_width);
			for (int ow = 0; ow < layer->__output_width; ++ow) {
				layer->delta_bias[mc][oh][ow] = 0;
			}
		}
	}
}

void Convolution_train_destruct(Convolution* layer) {
	for (int c = 0; c < layer->channels; ++c) {
		for (int h = 0; h < layer->height; ++h) {
			free(layer->err[c][h]);
		}
		free(layer->err[c]);
	}
	free(layer->err);

    for (int mc = 0; mc < layer->matrix_c; ++mc) {
	    for (int c = 0; c < layer->channels; ++c) {
		    for (int mh = 0; mh < layer->matrix_h; ++mh) {
		    	free(layer->delta_matrix[mc][c][mh]);
		    }
		    free(layer->delta_matrix[mc][c]);
		}
		free(layer->delta_matrix[mc]);
	}
	free(layer->delta_matrix);

	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			free(layer->delta_bias[mc][oh]);
		}
		free(layer->delta_bias[mc]);
	}
	free(layer->delta_bias);
}

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
				layer->input[c][h + layer->padding][w + layer->padding] = input[c][h][w];
			}
		}
	}
	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			for (int ow = 0; ow < layer->__output_width; ++ow) {
				layer->output[mc][oh][ow] = layer->bias[mc][oh][ow];
				for (int c = 0; c < layer->channels; ++c) {
					for (int mh = 0; mh < layer->matrix_h; ++mh) {
						for (int mw = 0; mw < layer->matrix_w; ++mw) {
							layer->output[mc][oh][ow] += layer->input[c][oh * layer->stride_h + mh][ow * layer->stride_w + mw] * layer->matrix[mc][c][mh][mw];
						}
					}
				}
				layer->output[mc][oh][ow] = activation(layer->output[mc][oh][ow]);
			}
		}
	}
}

void Convolution_backward(Convolution* layer, double*** output) {
	for (int c = 0; c < layer->channels; ++c) {
		for (int h = 0; h < layer->height; ++h) {
			for (int w = 0; w < layer->width; ++w) {
				layer->err[c][h][w] = 0;
			}
		}
	}
	

	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int c = 0; c < layer->channels; ++c) {
			for (int mh = 0; mh < layer->matrix_h; ++mh) {
				for (int mw = 0; mw < layer->matrix_w; ++mw) {
					layer->delta_matrix[mc][c][mh][mw] *= layer->momentum;
				}
			}
		}
	}

    // for mc in range(self.matrix_c):
    //     for hh in range(1 + (self.height - self.matrix_h) // self.stride_h):
    //         for ww in range(1 + (self.width - self.matrix_w) // self.stride_w):
    //             for c in range(self.channels):
    //                 for mh in range(self.matrix_h):
    //                     for mw in range(self.matrix_w):
    //                         self.delta_w[mc][c][mh][mw] += -self.learning_rate * self.data_input[c][hh * self.stride_h + mh][ww * self.stride_w + mw] * err[mc][hh][ww]
    //             self.delta_bias[mc][hh][ww] = -self.learning_rate * err[mc][hh][ww] + self.momentum * self.delta_bias[mc][hh][ww]
    //             self.bias[mc][hh][ww] += self.delta_bias[mc][hh][ww]

    // for mc in range(self.matrix_c):
    //     for c in range(self.channels):
    //         for mh in range(self.matrix_h):
    //             for mw in range(self.matrix_w):
    //                 self.w[mc][c][mh][mw] += self.delta_w[mc][c][mh][mw]

	for (int mc = 0; mc < layer->matrix_c; ++mc) {
		for (int oh = 0; oh < layer->__output_height; ++oh) {
			for (int ow = 0; ow < layer->__output_width; ++ow) {
				layer->delta_bias[mc][oh][ow] *= layer->momentum;
			}
		}
	}
	
    // for mc in range(self.matrix_c):
    //     for hh in range(1 + (self.height - self.matrix_h) // self.stride_h):
    //         for ww in range(1 + (self.width - self.matrix_w) // self.stride_w):
    //             for c in range(self.channels):
    //                 for mh in range(self.matrix_h):
    //                     for mw in range(self.matrix_w):
    //                         if self.padding <= hh * self.stride_h + mh < self.height - self.padding:
    //                             if self.padding <= ww * self.stride_w + mw < self.width - self.padding:
    //                                 self.err[c][hh * self.stride_h + mh - self.padding][ww * self.stride_w + mw - self.padding] += err[mc][hh][ww] * self.w[mc][c][mh][mw]  # * self.activation.df(self.x[mc][hh][ww])

}