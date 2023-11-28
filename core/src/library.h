#define pi 3.141592
#define e 2.718281


double random(double start, double end);
double random_int(int start, int end);


double Sigmoid_f(double x);
double Sigmoid_df(double x);

double ReLU_f(double x);
double ReLU_df(double x);

void SoftMax_f(int length, double* x, double* y);
double SoftMax_df(double x);

double MSE_f(double ideal, double output);
double MSE_df(double ideal, double output);