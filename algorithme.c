#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "algorithme.h"


//c'est la boucle principale de calcul, pour calculer tous les pixels de la nouvelle image
//l est le flag permettant de savoir si on calcule la vue gauche ou droite
//si l=+1 on fait la vue gauche, si l=-1 on fait la vue droite
void algorithme (BMP* data_out, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float A, float B){

  int i, j, iref;
  unsigned char r, g, b;
  
  for (j=0; j<height; j++){
    for (i=0; i<width; i++){
      iref = calcul_pixel(i, j, data_image, data_depth, width, height, ALPHA, l, A, B);
      if (iref>=0) {
        BMP_GetPixelRGB(data_image,iref,j,&r,&g,&b) ;
        BMP_SetPixelRGB(data_out,i,j,r,g,b) ;
      } else {
          BMP_SetPixelRGB(data_out,i,j,0,0,0) ;
      }
    }
  }

}

int calcul_pixel (int i, int j, BMP* data_image, BMP* data_depth, int width, int height, float ALPHA, int l, float A, float B){

  float NEW_DELTA = DELTA/ALPHA;
  
  float z_theorique = -NEW_DELTA;
  unsigned char ztemp, ztemp2, ztempref, ztempref2;
  int f = 1;
  int k = i-l*NEW_DELTA;
  int kmin = i-NEW_DELTA;
  int kmax = i+NEW_DELTA;
  int lastk;
  unsigned char zimg,zimgp ;
  float zref,zrefp,lastz ;
  int kp;
  kp = k-l;
  
  if (kmin<0) kmin=0;
  if (kmax>=width) kmax = width-1;
  if (k>=width) { k = width-1 ; z_theorique = i-width ;}
  if (k<0){ k=0; z_theorique=-i ;}
  if (kp<0) kp = 0 ;
  if (kp >= width) kp = width-1 ;

  while (k>=kmin && k<=kmax){
    BMP_GetPixelIndex(data_depth,k,j,&zimg) ;
    BMP_GetPixelIndex(data_depth,kp,j,&zimgp) ;
               
    zref  = A*zimg/ALPHA + B/ALPHA; 
    zrefp = A*zimgp/ALPHA + B/ALPHA; 

    if (fabs(z_theorique-zref)<0.5){
      return k;
    } 
    else if (z_theorique > zref && f==+1){
      if (fabs(zref - zrefp) < DZREF){
        return k;
      }
    }
    f = (z_theorique < zref);
    if(z_theorique < zref) {
			lastz = fabs(zref-z_theorique) ; 
			lastk = k; 
			BMP_GetPixelIndex(data_depth,k,j,&ztemp) ; 
			ztempref = A*ztemp/ALPHA + B/ALPHA; 
		}
    z_theorique = z_theorique+1.0;
    k = k+l;
    kp = k-l;
  }

  //on est dans un trou donc on fait l'effet miroir
  BMP_GetPixelIndex(data_depth, lastk-l*lastz ,j,&ztemp2) ;
  ztempref2  = A*ztemp2/ALPHA + B/ALPHA;

  if (fabs(ztempref2-ztempref)>4.0) return lastk;
  else return lastk-l*lastz;
}
