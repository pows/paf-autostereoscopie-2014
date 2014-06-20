

loader: algorithme.c loader.c qdbmp/qdbmp.c
	gcc -o loader loader.c algorithme.c qdbmp/qdbmp.c -I. -Iqdbmp -Wall 


chess: loader
	./loader chessRGB.bmp chessDEPTH.bmp 

lara: loader
	./loader lara_img.bmp lara_depth.bmp

coke: loader
	./loader coke_img.bmp coke_depth.bmp

clean:
	rm loader
