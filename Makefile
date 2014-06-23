

loader: algorithme.c loader.c qdbmp/qdbmp.c
	gcc -o loader loader.c algorithme.c qdbmp/qdbmp.c -I. -Iqdbmp -Wall 


chess: loader
	./loader chessRGB.bmp chessDEPTH.bmp 

lara: loader
	./loader lara_img.bmp lara_depth.bmp

coke: loader
	./loader coke_img.bmp coke_depth.bmp

cubic: loader
	./loader cubic_img.bmp cubic_depth.bmp

parrot: loader
	./loader parrot_img.bmp parrot_depth.bmp

fruits: loader
	./loader fruits_img.bmp fruits_depth.bmp

amelioration: amelioration.c
	gcc -o amelioration amelioration.c qdbmp/qdbmp.c -I. -Iqdbmp -Wall -lm

clean:
	rm loader
