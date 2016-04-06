/*
* 	SONEA Andreea 333CB
* 			TEMA 1 ALGORITMI PARALELI SI DISTRIBUITI
*/


#include "homework1.h"

int num_threads;
int resolution;

/*
* Functia elibereaza memoria alocata pentru imagine.
*/
void freeImage(image *img) {
	int i, j;

	for (i = 0; i < img->height; i++) {
		free(img->color[i]);
	}
	free(img->color);
}

/*
* Functia initializeaza antetul imaginii si aloca memorie pentru pixeli,
* acestia avand o culoare/pixel.
*/
void initialize(image *img) {
	int i;

	img->type = 5;
	img->width = resolution;
	img->height = resolution;
	img->maxVal = 255;

	img->color = malloc(img->height * sizeof(unsigned char *));
	if (img->color == NULL) {
		//printf("Eroare alocare memorie\n");
		exit(1);
	}
	for (i = 0; i < img->height; i++) {
		img->color[i] = malloc(img->width * sizeof(unsigned char));
		if (img->color[i] == NULL) {
			printf("Eroare alocare memorie\n");
			free(img->color);
			return;
		}
	}
}

/*
* Reruneaza distanta de la un punct la o dreapta a carei ecuatie
* este cunoscuta.
*/
double getDistance(int x0, int y0) {
	return (double)fabs(-1*x0 + 2*y0)/ sqrt(5);
}

/*
* Functia calculeaza "dimensiunea unui pixel" in functie de rezolutia data, 
* parcurge fiecare pixel, calculeaza distanta de la el la dreapta si coloreaza
* pixel-ul nou, calculand coordonatele lui in functie de rezolutie.
*/
void render(image *im) {
	int pixel_size = resolution / 100;
	int i, j, x, y, color;
	int N = 100;

	#pragma omp parallel for 	\
		private(i, j, x, y, color)	\
		num_threads(num_threads) 	\
		schedule(dynamic, 10)
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			// calcul distanta
			if (getDistance(j, i) < 3) {
				color = 0;
			}
			else color = 255;
			// colorarea noului "pixel"
			for (x = 0; x < pixel_size; x++) {
				for (y = 0; y < pixel_size; y++) {
					im->color[i * pixel_size + x][j * pixel_size + y] = color;
				}
			}	
		}
	}
}

/*
* Functia afiseaza
*/
void writeWB(FILE *f, image *img) {
	int i, j;
	unsigned char *buffer = malloc(img->height * img->width);
	int poz = 0;

	for (i =  img->height - 1; i >= 0; i--) {
		for (j = 0; j < img->width; j++) {
			buffer[poz++] = img->color[i][j];
		}
	}
	fwrite(buffer, 1, img->height * img->width, f);
	free(buffer);
}

/*
* Functia scrie in fisier antetul si valorile corespunzatoare
* pixel-ilor. Este dezalocata memoria ocupata de imagine.
*/
void writeData(const char * fileName, image *img) {
	FILE *f = fopen(fileName, "w");

	if (f == NULL) {
		//printf("Eroare deschidere fisier\n");
		exit(1);
	}
	// scriere antet
    fprintf(f, "P%d\n", img->type);	
    fprintf(f, "%d %d\n", img->width, img->height);
    fprintf(f, "%d\n", img->maxVal);

    //scriere pixeli
	writeWB(f, img);
	fclose(f);
	freeImage(img);
}
