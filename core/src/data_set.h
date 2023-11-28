typedef struct DataSet {
	int __current_length__;
	int length;
	int input_length;
	int output_length;
	double** input;
	double** output;
} DataSet;

DataSet* DataSet_construct(int length, int input_length, int output_length);
void DataSet_destruct(DataSet* data_set);

void DataSet_add(DataSet* data_set, double data[]);
