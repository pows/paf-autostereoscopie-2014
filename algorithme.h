#include "qdbmp.h"

int calcul_pixel (int i, int j, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float a, float b);

void algorithme (BMP* data_out, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float a, float b);
