#include <stdlib.h>

#include "data_set.h"


DataSet *DataSet_construct(int length, int input_length, int output_length) {
	DataSet *data_set = malloc(sizeof(DataSet));

	data_set->__current_length__ = 0;
	data_set->length = length;
	data_set->input_length = input_length;
	data_set->output_length = output_length;
	data_set->input = (double**)malloc(sizeof(double*) * length);
	data_set->output = (double**)malloc(sizeof(double*) * length);

	for (int i = 0; i < length; ++i) {
		data_set->input[i] = (double*)malloc(sizeof(double) * input_length);
		data_set->output[i] = (double*)malloc(sizeof(double) * output_length);
	}

	return data_set;
}

void DataSet_destruct(DataSet *data_set) {
	for (int i = 0; i < data_set->length; ++i) {
		free(data_set->input[i]);
		free(data_set->output[i]);
	}
	free(data_set->input);
	free(data_set->output);
	free(data_set);
}

void DataSet_add(DataSet *data_set, double data[]) {
	if (data_set->__current_length__ == data_set->length) {
		++data_set->length;
		++data_set->__current_length__;

		data_set->input = (double**)realloc(data_set->input, data_set->length * sizeof(double*));
		data_set->input[data_set->length - 1] = (double*)malloc(sizeof(double) * data_set->input_length);

		data_set->output = (double**)realloc(data_set->output, data_set->length * sizeof(double*));
		data_set->output[data_set->length - 1] = (double*)malloc(sizeof(double) * data_set->output_length);
	} else {
		++data_set->__current_length__;
	}

	for (int i = 0; i < data_set->input_length; ++i) {
		data_set->input[data_set->__current_length__ - 1][i] = data[i];
	}

	for (int i = 0; i < data_set->output_length; ++i) {
		data_set->output[data_set->__current_length__ - 1][i] = data[data_set->input_length + i];
	}
}
