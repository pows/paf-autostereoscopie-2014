#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithme.h>

#define DELTA 32
#define DECRAN 80.0 // Distance écran - utilisateur (en cm)
#define P 10.0 //plage de perception de la profondeur (en centimètre ~ profondeur du cube)
#define distance_inter_oculaire 6.5 // distance entre les yeux (en degrés)
#define Z0 0.0 // position de la face avant du cube dans l'espace
#define HAUTEUR_IMAGE 900 // hauteur de l’image
#define LARGEUR_IMAGE 1440 // largeur de l’image
#define DZREF 2.0 // distance caractéristique permettant de différencier un étirement d'un trou
#define K 0.7 //facteur de shrink d'échelle des z = 1/(sqrt(2)*cos(ALPHA))
#define pixel_cm 0.03 // largeur d'un pixel en cm

           
               
       

// Renvoie l'indice réel du pixel à utiliser
int calcul_pixel(int i, int j, BMP * data_image, BMP * data_depth, int width, int height){

	
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
                
                zref  = (distance_inter_oculaire/DECRAN)*((zimg* (P/256.0)+Z0)/pixel_cm);
                zrefp = (distance_inter_oculaire/DECRAN)*((zimgp*(P/256.0)+Z0)/pixel_cm);

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
	}
        // On est dans un trou
        
	return lastk-lastz;
}

unsigned char * algorithme(BMP* data_out,BMP * data_image, BMP * data_depth, int width, int height){

	
	int i ;
	int j ;
        int iref ;
        unsigned char r,g,b ;
        
	for (j=0; j< height ; j++){
		for(i=0; i< width; i++){
			iref = calcul_pixel( i, j,  data_image,  data_depth, width, height);
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
