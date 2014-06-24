#include "qdbmp.h"
#define DELTA 64
#define DECRAN 80.0 // Distance écran - utilisateur (en cm)
#define P 10.0 //plage de perception de la profondeur (en centimètre ~ profondeur du cube)
#define distance_inter_oculaire 6.5 // distance entre les yeux (en degrés)
#define Z0 5.0 // position de la face avant du cube dans l'espace
#define HAUTEUR_IMAGE 900 // hauteur de l’image
#define LARGEUR_IMAGE 1440 // largeur de l’image
#define DZREF 2.0 // distance caractéristique permettant de différencier un étirement d'un trou
#define pixel_cm 0.03 // largeur d'un pixel en cm

int calcul_pixel (int i, int j, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float a, float b);

void algorithme (BMP* data_out, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float a, float b);
