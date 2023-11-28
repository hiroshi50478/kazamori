#include <stdlib.h>
#include <math.h>

#include "library.h"


// Function that return random double in [start; end]
double random(double start, double end) {
	return start + (double)rand() / (double)RAND_MAX * (end - start);
}

// Function that return random int in [start; end]
double random_int(int start, int end) {
	return start + rand() * (end - start) / RAND_MAX;
}


// Activation functions
double Sigmoid_f(double x) {
	return 1 / (1 + pow(e, -x));
}

double Sigmoid_df(double x) {
	return x * (1 - x);
}

double ReLU_f(double x) {
	if (x > 0) {
		return x;
	}
	return 0;
}

double ReLU_df(double x) {
	if (x > 0) {
		return 1;
	}
	return 0;
}

void SoftMax_f(int length, double* x, double* y) {
	double sum = 0;
	for (int i = 0; i < length; ++i) {
		sum += pow(e, x[i]);
	}
	for (int i = 0; i < length; ++i) {
		y[i] = pow(e, x[i]) / sum;
	}
}

double SoftMax_df(double x) {
	return x * (1 - x);
}


// Loss functions
double MSE_f(double ideal, double output) {
    return (output - ideal) * (output - ideal);
}

double MSE_df(double ideal, double output) {
	return 2 * (output - ideal);
}