/*
* 	SONEA Andreea 333CB
* 			TEMA 1 ALGORITMI PARALELI SI DISTRIBUITI
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef struct Image{
	int type;
	int width, height, maxVal;
	unsigned char **color;
} image;
