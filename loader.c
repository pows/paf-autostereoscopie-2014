#include "qdbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include "algorithme.h"

#define DELTA 64
#define DECRAN 80.0 // Distance écran - utilisateur (en cm)
#define P 10.0 //plage de perception de la profondeur (en centimètre ~ profondeur du cube)
#define distance_inter_oculaire 6.5 // distance entre les yeux (en degrés)
#define Z0 5.0 // position de la face avant du cube dans l'espace
#define HAUTEUR_IMAGE 900 // hauteur de l’image
#define LARGEUR_IMAGE 1440 // largeur de l’image
#define DZREF 2.0 // distance caractéristique permettant de différencier un étirement d'un trou
#define pixel_cm 0.03 // largeur d'un pixel en cm

int main (int argc, char*argv[]){

  float A = ((distance_inter_oculaire/DECRAN))*((P/256.0)/pixel_cm);
  float B = ((distance_inter_oculaire/DECRAN))*Z0/pixel_cm;

  int width;
  int height;
  BMP * data_image ;
  BMP * data_depth ;
  BMP * data_out1 ;
  BMP * data_out2 ;
  BMP * data_out3 ;
  BMP * data_out4 ;
  BMP * data_out5 ;
  BMP * data_out6;
  BMP * data_out7 ;
  BMP * data_out8 ;


  /* Check arguments */
	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s <rgb file> <depth file>\n", argv[ 0 ] );
		return 0;
	}

  // Lecture de l'image de référence
  data_image =  BMP_ReadFile(argv[1]);
  BMP_CHECK_ERROR( stdout, -1 );
  
  // Lecture de l'image de profondeur
  data_depth =  BMP_ReadFile(argv[2]);
  BMP_CHECK_ERROR( stdout, -1 );

  // Récupération des dimenssion
  width = BMP_GetWidth(data_image) ;
  height = BMP_GetHeight(data_image) ;

  // Création des 4 images futures
  data_out1 = BMP_Create(width,height,24) ;
  data_out2 = BMP_Create(width,height,24) ;
  data_out3 = BMP_Create(width,height,24) ;
  data_out4 = BMP_Create(width,height,24) ;
  data_out5 = BMP_Create(width,height,24) ;
  data_out6 = BMP_Create(width,height,24) ;
  data_out7 = BMP_Create(width,height,24) ;
  data_out8 = BMP_Create(width,height,24) ;

  // Execution de l'algorithme
  algorithme (data_out1,data_image, data_depth, width, height, 1.0, 1, A, B);
  algorithme (data_out2,data_image, data_depth, width, height, 1.25, 1, A, B);
  algorithme (data_out3,data_image, data_depth, width, height, 2.0, 1, A, B);
  algorithme (data_out4,data_image, data_depth, width, height, 4.0, 1, A, B);
  algorithme (data_out5,data_image, data_depth, width, height, 4.0, -1, A, B);
  algorithme (data_out6,data_image, data_depth, width, height, 2.0, -1, A, B);
  algorithme (data_out7,data_image, data_depth, width, height, 1.25, -1, A, B);
  algorithme (data_out8,data_image, data_depth, width, height, 1.0, -1, A, B);
  

  // Ecriture de l'image résultante
  BMP_WriteFile(data_out1,"image1.bmp") ;
  BMP_WriteFile(data_out2,"image2.bmp") ;
  BMP_WriteFile(data_out3,"image3.bmp") ;
  BMP_WriteFile(data_out4,"image4.bmp") ;
  BMP_WriteFile(data_out5,"image5.bmp") ;
  BMP_WriteFile(data_out6,"image6.bmp") ;
  BMP_WriteFile(data_out7,"image7.bmp") ;
  BMP_WriteFile(data_out8,"image8.bmp") ;

  return 0;

}
