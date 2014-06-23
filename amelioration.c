#include "qdbmp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DELTA 5

void ameliorer(BMP* data_out, BMP* data_image, BMP* data_depth, int height, int width);
unsigned char ameliorer_pixel(int i, int j, BMP* data_out,BMP* data_image, BMP* data_depth, int width);

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
  data_out = BMP_Create(width,height,24) ;

  //execution de l'algorithme d'amelioration
  int i, j; 
  unsigned char val ;
  for (j=0; j<height; j++){
    for (i = 0; i<width; i++){ 
      val = ameliorer_pixel(i,j,data_out,data_image,data_depth,width) ;
      //printf("%d\n",val) ;
      BMP_SetPixelRGB(data_out, i, j, val,val,val);
    }
  }
  //ameliorer(data_out,data_image, data_depth, height, width);

  //ecriture de l'image résultante
  BMP_WriteFile(data_out, "depth.bmp");

  return 0 ;
}


void ameliorer(BMP* data_out, BMP* data_image, BMP* data_depth, int height, int width){
  
  int i, j; 
  unsigned char val ;
  for (j=0; j<height; j++){
    for (i = 0; i<width; i++){ 
      val = ameliorer_pixel(i,j,data_out,data_image,data_depth,width) ;
      printf("%d\n",val) ;
      BMP_SetPixelIndex(data_out, i, j, val);
    }
  }
}



unsigned char ameliorer_pixel(int i, int j, BMP* data_out, BMP* data_image, BMP* data_depth, int  width){

	unsigned char r0, g0, b0, r1, g1, b1, r2, g2, b2;
        float fr0, fg0, fb0, fr1, fg1, fb1, fr2, fg2, fb2;
        unsigned char z0,z1,z2 ;

        BMP_GetPixelIndex(data_depth, i, j, &z0);
        if (i==0) return z0;
	if (i==width) return z0;
        //printf("%d %d %d \n",z0,z1,z2) ;
      
        unsigned char zz, zzz; 
        BMP_GetPixelRGB(data_out,i-1,j,&z1,&zz,&zzz) ;
        //BMP_GetPixelIndex(data_depth, i-1, j, &z1);
        BMP_GetPixelIndex(data_depth, i+1, j, &z2);

	BMP_GetPixelRGB(data_image, i-1, j, &r1, &g1, &b1);
	BMP_GetPixelRGB(data_image, i+1, j, &r2, &g2, &b2);
	BMP_GetPixelRGB(data_image, i, j, &r0, &g0, &b0);
        fr0 = r0 ;
        fr1 = r1 ;
        fr2 = r2 ;
        fg0 = g0 ;
        fg1 = g1 ;
        fg2 = g2 ;
        fb0 = b0 ;
        fb1 = b1 ;
        fb2 = b2 ;
       // printf("%f %f %f \n",fr0,fg0,fb0) ;
       // printf("%f %f %f \n",fr1,fg1,fb1) ;
       // printf("%f %f %f \n",fr2,fg2,fb2) ;
       
        
        float delta12, delta10, delta02 ;
        
        delta12 = sqrt( pow(fr1-fr2,2) + pow(fg1-fg2,2) + pow(fb1-fb2,2)) ;
        delta10 = sqrt( pow(fr1-fr0,2) + pow(fg1-fg0,2) + pow(fb1-fb0,2)) ;
	delta02 = sqrt( pow(fr0-fr2,2) + pow(fg0-fg2,2) + pow(fb0-fb2,2)) ;
	//printf("%f %f %f \n",delta12,delta10,delta02) ;
        if ( delta12  >  DELTA) 
	{
		if (delta10 < delta02)
		{
			return z1;
		} else return z2;
	} else return z0;

}


