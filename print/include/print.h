void DataSet_print(DataSet* data_set);

void FullConnected_print_weights(FullConnected* layer);
void FullConnected_print_biases(FullConnected* layer);
void FullConnected_print_x(FullConnected* layer);
void FullConnected_print_delta_weights(FullConnected* layer);
void FullConnected_print_delta_biases(FullConnected* layer);
void FullConnected_print_err(FullConnected* layer);

void ImageSet_print_input(ImageSet* image_set);
void ImageSet_print_output(ImageSet* image_set);

void MaxPooling_print_input(MaxPooling* layer);
void MaxPooling_print_output(MaxPooling* layer);
void MaxPooling_print_err(MaxPooling* layer);

void Convolution_print_output(Convolution* layer);
