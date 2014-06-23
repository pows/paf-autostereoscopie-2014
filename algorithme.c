#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "algorithme.h"

#define DELTA 64
#define DECRAN 80.0 // Distance écran - utilisateur (en cm)
#define P 10.0 //plage de perception de la profondeur (en centimètre ~ profondeur du cube)
#define distance_inter_oculaire 6.5 // distance entre les yeux (en degrés)
#define Z0 0.0 // position de la face avant du cube dans l'espace
#define HAUTEUR_IMAGE 900 // hauteur de l’image
#define LARGEUR_IMAGE 1440 // largeur de l’image
#define DZREF 2.0 // distance caractéristique permettant de différencier un étirement d'un trou
#define pixel_cm 0.03 // largeur d'un pixel en cm


// Renvoie l'indice réel du pixel à utiliser pour générer une vue gauche
int calcul_pixel_gauche(int i, int j, BMP * data_image, BMP * data_depth, int width, int height, float ALPHA){

	
	float z_theorique = -DELTA;
	int f = 1;
	int k = i-DELTA;
        int kmax = i+DELTA ;
	int lastk;
        unsigned char zimg,zimgp ;
        float zref,zrefp,lastz ;
        if(k<0){ k=0; z_theorique=-i ;} 
        int kp ;
        kp = k-1 ;
        if(kp<0) kp = 0 ;
        if(kmax >= width) kmax = width ;
	while (k<kmax ){
                BMP_GetPixelIndex(data_depth,k,j,&zimg) ;
                BMP_GetPixelIndex(data_depth,kp,j,&zimgp) ;
                
                zref  = ((distance_inter_oculaire/DECRAN)/(ALPHA))*((zimg* (P/256.0)+Z0)/pixel_cm);
                zrefp = ((distance_inter_oculaire/DECRAN)/(ALPHA))*((zimgp*(P/256.0)+Z0)/pixel_cm);

		if (fabs(z_theorique-zref)<0.5){
			return k;
		} 
		else if (z_theorique > zref && f==+1){
			if (fabs(zref - zrefp) < DZREF){
				return k;
			}
		}
		f = (z_theorique < zref);
                if(z_theorique < zref) {lastz = fabs(zref-z_theorique) ; lastk = k;}
		z_theorique = z_theorique+1.0;
		k++;
		kp=k-1;
	}
        // On est dans un trou
        
	return lastk-lastz;
}

// Renvoie l'indice réel du pixel à utiliser pour générer une vue droite
int calcul_pixel_droite(int i, int j, BMP * data_image, BMP * data_depth, int width, int height, float ALPHA){

	
	float z_theorique = -DELTA;
	int f = 1;
	int kmin = i-DELTA;
        int k = i+DELTA ;
	int lastk;
        unsigned char zimg,zimgp ;
        float zref,zrefp,lastz ;
        if(k >= width) { k = width-1 ; z_theorique = i-width ;}
        int kp ;
        kp = k+1 ;
        if(kp >= width) kp = width-1 ;
        if(kmin<0)kmin=0 ; 

	while (k>=kmin){
                BMP_GetPixelIndex(data_depth,k,j,&zimg) ;
                BMP_GetPixelIndex(data_depth,kp,j,&zimgp) ;
                
                zref  = (distance_inter_oculaire/DECRAN)/(ALPHA)*((zimg* (P/256.0)+Z0)/pixel_cm);
                zrefp = (distance_inter_oculaire/DECRAN)/(ALPHA)*((zimgp*(P/256.0)+Z0)/pixel_cm);

		if (fabs(z_theorique-zref)<0.5){
			return k;
		} 
		else if (z_theorique > zref && f==+1){
			if (fabs(zref - zrefp) < DZREF){
				return k;
			}
		}
		f = (z_theorique < zref);
                if(z_theorique < zref) {lastz = fabs(zref-z_theorique) ; lastk = k;}
		z_theorique = z_theorique+1.0;
		k--;
		kp=k+1;
	}
        // On est dans un trou
        
	return lastk+lastz;
}



unsigned char * algorithme(BMP* data_out,BMP * data_image, BMP * data_depth, int width, int height, int f, float ALPHA){
//f est le flag permettant de savoir si on calcule une vue droite ou une vue gauche : si f=-1 on fait la droite, si f=1 on fait la gauche
	
	int i ;
	int j ;
        int iref ;
        unsigned char r,g,b ;
        
	for (j=0; j< height ; j++){
		for(i=0; i< width; i++){
			if (f==-1) iref = calcul_pixel_droite( i, j,  data_image,  data_depth, width, height, ALPHA);
			else if(f==1) iref = calcul_pixel_gauche( i, j,  data_image,  data_depth, width, height, ALPHA);
                        if(iref >=0) {
                          BMP_GetPixelRGB(data_image,iref,j,&r,&g,&b) ;
                          BMP_SetPixelRGB(data_out,i,j,r,g,b) ;
                        } else {
                          BMP_SetPixelRGB(data_out,i,j,0,0,0) ;
                        }
		}
	}

	return 0;
}


