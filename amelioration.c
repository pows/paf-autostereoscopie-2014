#include "qdbmp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DELTA 20

BMP* ameliorer(BMP* data_image, BMP* data_depth,int height, int width);
int ameliorer_pixel(int i, int j, BMP* data_image, int width);

int main (int argc, char* argv[]){

  int width, height;
  BMP* data_out;
  BMP* data_image; 
  BMP* data_depth;

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

  // Création de l'image future
  data_out = BMP_Create(width,height,8) ;

  //execution de l'algorithme d'amelioration
  data_out = ameliorer(data_image, data_depth, height, width);

  //ecriture de l'image résultante
  BMP_WriteFile(data_out, "depth.bmp");

 
}


BMP* ameliorer(BMP* data_image, BMP* data_depth, int height, int width){
  
  int i, j; 
  float val;
  BMP* data_out;
  for (j=0; j<height; j++){
    for (i = 0; i<width; i++){ 
      BMP_GetPixelIndex(data_depth, j, i, &val);
      BMP_SetPixelIndex(data_out, j, ameliorer_pixel(i,j,data_image, val), width);
    }
  }

}



int ameliorer_pixel(int i, int j, BMP* data_image, int width){

	unsigned char r0, g0, b0, r1, g1, b1, r2, g2, b2;
	BMP_GetPixelRGB(data_image, i-1, j, &r1, &g1, &b1);
	BMP_GetPixelRGB(data_image, i+1, j, &r2, &g2, &b2);
	BMP_GetPixelRGB(data_image, i, j, &r0, &g0, &b0);


	if (i==0) return i;
	if (j==width) return i;
	if ( sqrt( pow((fabs((unsigned int)r1-(unsigned int)r2)),2) + pow((fabs((unsigned int)g1-(unsigned int)g2)),2) + pow((fabs((unsigned int)b1-(unsigned int)b2)),2) ) >  DELTA) 
	{
		if ( sqrt( pow((fabs((unsigned int)r0-(unsigned int)r1)),2) + pow((fabs((unsigned int)g0-(unsigned int)g1)),2) + pow((fabs((unsigned int)b0-(unsigned int)b1)),2 )) < sqrt( pow((fabs((unsigned int)r0-(unsigned int)r2)),2) + pow((fabs((unsigned int)g0-(unsigned int)g2)),2) + pow((fabs((unsigned int)b0-(unsigned int)b2)),2) )) 
		{
			return i-1;
		} else return i+1;
	} return i;

}


