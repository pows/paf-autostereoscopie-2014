
#include "qdbmp.h"

unsigned char * algorithme(BMP* data_out, BMP* data_image, BMP * data_depth, int width, int height);

int calcul_pixel(int i, int j, BMP* data_image, BMP* data_depth, int width, int height);
