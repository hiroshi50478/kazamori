typedef struct Convolution {
	int channels;
	int height;
	int width;
	int matrix_c;
	int matrix_h;
	int matrix_w;
	int padding;
	int stride_h;
	int stride_w;

	double*** input;
	double*** output;
	double**** matrix;
	double*** bias;
	double*** err;

	double learning_rate;
	double momentum;
} Convolution;


Convolution* Convolution_construct(int channels, int height, int width, int matrix_c, int matrix_h, int matrix_w, int padding, int stride_h, int stride_w);
void Convolution_destruct(Convolution* layer);

void Convolution_train_construct(Convolution* layer, double learning_rate, double momentum);
void Convolution_train_destruct(Convolution* layer);

void Convolution_forward(Convolution* layer, double*** input);
void Convolution_backward(Convolution* layer, double*** output);
