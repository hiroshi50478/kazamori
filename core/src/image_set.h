typedef struct ImageSet {
	int __current_length__;
	int length;
	int channels;
	int height;
	int width;
	double**** input;
	int output_length;
	double** output;
} ImageSet;

ImageSet* ImageSet_construct(int length, int channels, int height, int width, int output_length);
void ImageSet_destruct(ImageSet* image_set);

void ImageSet_add(ImageSet* image_set, double input[], double output[]);