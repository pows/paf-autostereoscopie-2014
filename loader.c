#include "qdbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include "algorithme.h"

//#include <GL/glew.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
//#include <GL/glext.h>



#define DELTA 16
#define DECRAN 80 // Distance écran - utilisateur (en cm)
#define P 20 //plage de perception de la profondeur (en centimètre ~ profondeur du cube)
#define ALPHA 4.6 // Angle entre les yeux en degré
#define Z0 0 // position du plan de l’écran dans le cube entre 0 et 255
#define HAUTEUR_IMAGE 900 // hauteur de l’image
#define LARGEUR_IMAGE 1440 // largeur de l’image
#define ZREF 5 // distance caractéristique permettant de différencier un étirement d'un trou
#define distance_inter_oculaire1 6.5
#define distance_inter_oculaire2 13.0


//void CreateAndSaveBMP(const char *fileName, unsigned int width, unsigned int height, unsigned int nb_components, unsigned char *pixels);
//void WriteHeader(FILE *bmpfile, unsigned int width , unsigned int height);
//unsigned char * loadBMP ( FILE * img , int * width, int * height);
//void algorithme_couleur (char * data_image, char * data_depth, int width, int height);

int main (int argc, char *argv[])
{
  
  int width;
  int height;
  BMP * data_image ;
  BMP * data_depth ;
  BMP * data_out1 ;
  BMP * data_out2 ;
  BMP * data_out3 ;
  BMP * data_out4 ;

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
  
  // Execution de l'algorithme
  algorithme (data_out1,data_image, data_depth, width, height, 1, 1.0);
  algorithme (data_out2,data_image, data_depth, width, height, 1, 2.0);
  algorithme (data_out3,data_image, data_depth, width, height, -1, 2.0);
  algorithme (data_out4,data_image, data_depth, width, height, -1, 1.0);
  
  // Ecriture de l'image résultante
  BMP_WriteFile(data_out1,"image1.bmp") ;
  BMP_WriteFile(data_out2,"image2.bmp") ;
  BMP_WriteFile(data_out3,"image3.bmp") ;
  BMP_WriteFile(data_out4,"image4.bmp") ;

  return 0;
}
