/*
* 	SONEA Andreea 333CB
* 			TEMA 1 ALGORITMI PARALELI SI DISTRIBUITI
*/


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

typedef struct Image{
	int type;
	int width, height, maxVal;
	unsigned char **red;
	unsigned char **green;
	unsigned char **blue;
} image;