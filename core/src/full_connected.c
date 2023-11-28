#include <stdlib.h>

#include "library.h"
#include "full_connected.h"

#define activation_f(x) Sigmoid_f(x)
#define activation_df(x) Sigmoid_df(x)
#define loss_f(ideal, output) MSE_f(ideal, output)
#define loss_df(ideal, output) MSE_df(ideal, output)


FullConnected* FullConnected_construct(int length, int layers[]) {
	FullConnected* layer = (FullConnected*)malloc(sizeof(FullConnected));
	
	layer->length = length;

	layer->layers = (int*)malloc(sizeof(int) * layer->length);
	layer->x = (double**)malloc(sizeof(double*) * layer->length);
	for (int i = 0; i < layer->length; ++i) {
		layer->layers[i] = layers[i];
		layer->x[i] = (double*)malloc(sizeof(double) * layer->layers[i]);
	}

	layer->weights = (double***)malloc(sizeof(double**) * (layer->length - 1));
	layer->biases = (double**)malloc(sizeof(double*) * (layer->length - 1));
	for (int i = 0; i < layer->length - 1; ++i) {
		layer->weights[i] = (double**)malloc(sizeof(double*) * layer->layers[i]);
		layer->biases[i] = (double*)malloc(sizeof(double) * layer->layers[i + 1]);
		for (int j = 0; j < layer->layers[i]; ++j) {
			layer->weights[i][j] = (double*)malloc(sizeof(double) * layer->layers[i + 1]);
			layer->biases[i][j] = random(-1, 1);
			for (int l = 0; l < layer->layers[i + 1]; ++l) {
				layer->weights[i][j][l] = random(-1, 1);
			}
		}
	}

	return layer;
}

void FullConnected_destruct(FullConnected* layer) {
	free(layer->layers);

	for (int i = 0; i < layer->length; ++i) {
		free(layer->x[i]);
	}
	free(layer->x);

	for (int i = 0; i < layer->length - 1; ++i) {
		for (int j = 0; j < layer->layers[i]; ++j) {
			free(layer->weights[i][j]);
		}
		free(layer->weights[i]);
		free(layer->biases[i]);
	}
	free(layer->weights);
	free(layer->biases);
	
	free(layer);
}

void FullConnected_train_construct(FullConnected* layer, double learning_rate, double momentum) {
	layer->learning_rate = learning_rate;
	layer->momentum = momentum;

	layer->err = (double**)malloc(sizeof(double*) * layer->length);
	for (int i = 0; i < layer->length; ++i) {
		layer->err[i] = (double*)malloc(sizeof(double) * layer->layers[i]);
	}

	layer->delta_weights = (double***)malloc(sizeof(double**) * (layer->length - 1));
	layer->delta_biases = (double**)malloc(sizeof(double*) * (layer->length - 1));
	for (int i = 0; i < layer->length - 1; ++i) {
		layer->delta_weights[i] = (double**)malloc(sizeof(double*) * layer->layers[i]);
		layer->delta_biases[i] = (double*)malloc(sizeof(double) * layer->layers[i + 1]);
		for (int j = 0; j < layer->layers[i]; ++j) {
			layer->delta_weights[i][j] = (double*)malloc(sizeof(double) * layer->layers[i + 1]);
		}
	}
}

void FullConnected_train_destruct(FullConnected* layer) {
	for (int i = 0; i < layer->length; ++i) {
		free(layer->err[i]);
	}
	free(layer->err);

	for (int i = 0; i < layer->length - 1; ++i) {
		for (int j = 0; j < layer->layers[i]; ++j) {
			free(layer->delta_weights[i][j]);
		}
		free(layer->delta_weights[i]);
		free(layer->delta_biases[i]);
	}
	free(layer->delta_weights);
	free(layer->delta_biases);
}

void FullConnected_forward(FullConnected* layer, double* input) {
	for (int i = 0; i < layer->layers[0]; ++i) {
		layer->x[0][i] = input[i];
	}

	for (int i = 0; i < layer->length - 1; ++i) {
		for (int l = 0; l < layer->layers[i + 1]; ++l) {
			layer->x[i + 1][l] = layer->biases[i][l];
			for (int j = 0; j < layer->layers[i]; ++j) {
				layer->x[i + 1][l] += layer->weights[i][j][l] * layer->x[i][j];
			}
			if (i + 1 < layer->length - 1) {
				layer->x[i + 1][l] = activation_f(layer->x[i + 1][l]);
			}
		}
	}
	SoftMax_f(layer->layers[layer->length - 1], layer->x[layer->length - 1], layer->x[layer->length - 1]);
}

void FullConnected_backward(FullConnected* layer, double* output) {
	for (int i = 0; i < layer->layers[layer->length - 1]; ++i) {
		layer->err[layer->length - 1][i] = loss_df(output[i], layer->x[layer->length - 1][i]) * SoftMax_df(layer->x[layer->length - 1][i]);
	}

	for (int i = layer->length - 2; i > -1; --i) {
		for (int j = 0; j < layer->layers[i]; ++j) {
			layer->err[i][j] = 0;
			for (int l = 0; l < layer->layers[i + 1]; ++l) {
				layer->err[i][j] += layer->err[i + 1][l] * layer->weights[i][j][l] * activation_df(layer->x[i][j]);
			}
		}
	}

	for (int i = 0; i < layer->length - 1; ++i) {
		for (int l = 0; l < layer->layers[i + 1]; ++l) {
			for (int j = 0; j < layer->layers[i]; ++j) {
                layer->delta_weights[i][j][l] = -layer->learning_rate * layer->x[i][j] * layer->err[i + 1][l] + layer->momentum * layer->delta_weights[i][j][l];
                layer->weights[i][j][l] += layer->delta_weights[i][j][l];
			}
            
            layer->delta_biases[i][l] = -layer->learning_rate * layer->err[i + 1][l] + layer->momentum * layer->delta_biases[i][l];
            layer->biases[i][l] += layer->delta_biases[i][l];
		}
	}
}
