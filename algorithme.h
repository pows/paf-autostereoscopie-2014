
#include "qdbmp.h"

unsigned char * algorithme(BMP* data_out,BMP * data_image, BMP * data_depth, int width, int height, int f, float ALPHA);

int calcul_pixel_gauche(int i, int j, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA);

int calcul_pixel_droite(int i, int j, BMP * data_image, BMP * data_depth, int width, int height, float ALPHA);
