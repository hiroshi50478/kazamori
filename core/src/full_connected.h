typedef struct FullConnected {
	int length;
	int* layers;
	double learning_rate;
	double momentum;
	double** x;
	double*** weights;
	double** biases;
	double** err;
	double*** delta_weights;
	double** delta_biases;

} FullConnected;

FullConnected* FullConnected_construct(int layers_length, int layers[]);
void FullConnected_destruct(FullConnected* layer);

void FullConnected_train_construct(FullConnected* layer, double learning_rate, double momentum);
void FullConnected_train_destruct(FullConnected* layer);

void FullConnected_forward(FullConnected* layer, double* input);
void FullConnected_backward(FullConnected* layer, double* output);
