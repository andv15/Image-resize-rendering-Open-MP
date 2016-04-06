/*
* 	SONEA Andreea 333CB
* 			TEMA 1 ALGORITMI PARALELI SI DISTRIBUITI
*/



#include "homework.h"

int resize_factor;
int num_threads;
int CHUNK = 10;

/*
* Functia elibereaza memoria alocata pentru o imagine.
*/
void freeImage(image *img) {

	int i, j;
	for (i = 0; i < img->height; i++) {
		free(img->red[i]);
		free(img->green[i]);
		free(img->blue[i]);
	}
	free(img->red);
}

/*
* Functia scrie datele din o variabila image alb-negru intr-un
* buffer care va fi scris in fisierul de iesire.
*/
void writeWB(FILE *f, image *img) {
	int i, j, poz;
	unsigned char *buffer = malloc(img->height * img-> width);	

	poz = 0;
	for (i = 0; i < img->height; i++) {
		for (j = 0; j < img->width; j++) {
			// este afisata doar o "culoare", imaginea alb-negru
			// avand intr-un pixel o singura valoare
			buffer[poz++] = img->red[i][j];
		}
	}

	fwrite(buffer, 1, img->height * img-> width, f);
	free(buffer);
}


/*
* Functia scrie datele din o variabila image color intr-un
* buffer care va fi scris in fisierul de iesire.
*/
void writeColor(FILE *f, image *img) {
	int i, j, poz;
	unsigned char *buffer = malloc(img->height * img-> width * 3);	

	poz = 0;
	for (i = 0; i < img->height; i++) {
		for (j = 0; j < img->width; j++) {
			// RGB pentru imagini color
			buffer[poz++] = img->red[i][j];
			buffer[poz++] = img->green[i][j];
			buffer[poz++] = img->blue[i][j];
		}
	}
	fwrite(buffer, 1, img->height * img-> width * 3, f);
	// eliberare memorie buffer
	free(buffer);
}


/*
* Functia creaza fisierul de iesire pentru imaginea creata
* si afiseaza datele in el.
*/
void writeData(const char * fileName, image *img) {
	FILE *f = fopen(fileName, "w");

	if (f == NULL) {
		// eroare deschidere fisier
		exit(1);
	}

	// scriere antet
    fprintf(f, "P%d\n", img->type);	
    fprintf(f, "%d %d\n", img->width, img->height);
    fprintf(f, "%d\n", img->maxVal);

    // scriere date in fisier
	if (img->type == 5) {
		writeWB(f, img);
	}
	else writeColor(f, img);
	fclose(f);
	freeImage(img);
}

/*
* Functia aloca memorie pentru o variabila de tip image (RGB)
*
*/
void initImage(image *img) {
	int i;

	img->red = malloc(img->height * sizeof(unsigned char *));
	img->green = malloc(img->height * sizeof(unsigned char *));
	img->blue = malloc(img->height * sizeof(unsigned char *));

	if (img->red == NULL || img->green == NULL || img->blue == NULL) {
		// eroare alocare memorie
		exit(1);
	}
	for (i = 0; i < img->height; i++) {
		img->red[i] = malloc(img->width * sizeof(unsigned char));
		img->green[i] = malloc(img->width * sizeof(unsigned char));
		img->blue[i] = malloc(img->width * sizeof(unsigned char));

		if (img->red[i] == NULL || img->green[i] == NULL 
									|| img->blue[i] == NULL) {
			// eroare alocare memorie
			free(img->red);
			free(img->green);
			free(img->blue);
			exit(1);
		}
	}
}

/*
* Sunt citite datele dintr-un fisier .pgm intr-un buffer. 
* Valorile din buffer sunt stocate in structura image->red, 
* fisierele .pgm avand o singura culoare pe pixel.
*/
void readBW(FILE * f, image *img) {
	int i, j, poz;
	unsigned char *buffer = malloc(img->height * img-> width);
	fread(buffer, 1, img->height * img-> width, f);

	poz = 0;
	for (i = 0; i < img->height; i++) {
		for (j = 0; j < img->width; j++) {
			img->red[i][j] = buffer[poz++];
		}
	}
	free(buffer);
}

/*
* Sunt citite datele dintr-un fisier .pnm intr-un buffer. 
* Valorile din buffer sunt stocate in structura image->red, 
* image->green, image->blue, fisierele .pnm avand 3 culoari 
* pe pixel.
*/
void readColor(FILE * f, image *img) {
	int i, j, poz;
	unsigned char *buffer = malloc(img->height * img-> width * 3);
	fread(buffer, 1, img->height * img-> width * 3, f);;

	poz = 0;
	for (i = 0; i < img->height; i++) {
		for (j = 0; j < img->width; j++) {
			img->red[i][j] = buffer[poz++];
			img->green[i][j] = buffer[poz++];
			img->blue[i][j] = buffer[poz++];

		}
	}
	free(buffer);
}

/*
* Functia citeste antetul dintr-un fisier .pnm sau .pgm
* si apeleaza functia corespunzatoare pentru alocarea de  
* memorie si citirea "pixel-ilor".
*/
void readInput(const char * fileName, image *img) {
	FILE * f  = fopen(fileName, "r");
	char type[5];

	if (f == NULL) {
		// eroare deschidere fisier
		exit(1);
	}

	// citire antet
	fscanf(f, "%s", type);
	fscanf(f, "%d%d",&img->width, &img->height);
	fscanf(f, "%d", &img->maxVal);
	getc(f);
	// alocare memorie
	initImage(img);

	if (strcmp(type, "P5") == 0) {
		img->type = 5;
		// citire imagine .pgm
		readBW(f, img);
	}
	else	if (strcmp(type, "P6") == 0) {
				img->type = 6;
				// citire imagine .pnm
				readColor(f, img);
			}
			else	{
						freeImage(img);
						// format imagine incorect
						exit(1);
			}
	fclose(f);
}


/*
* Functia "micsoreaza" imaginea primita de un numar par de ori 
* prin calcularea mediei aritmetice a pixelilor vecini in 
* functie de resize_factor.
*/
void resize_odd(image *in, image * out) {
	int i, j, x, y, sum_red, sum_green, sum_blue;
	int resize = resize_factor;
	int o = resize_factor * resize_factor;

	#pragma omp parallel for 	\
		private(i,j,x,y,sum_red, sum_green, sum_blue) 	\
		num_threads(num_threads) 	\
		schedule(dynamic, CHUNK)
	// este parcursa imaginea noua
	for (i = 0; i < out->height; i++) {
		for( j = 0; j < out->width; j++) {
			sum_red = 0;
			sum_green = 0; 
			sum_blue = 0;  
			// sunt parcursi pixeli imaginii initiale care trebuiesc 
			// "restransi" in pixelul [i,j] si se calculeaza suma lor
			for (x = i * resize; x < i * resize + resize; x++) {
				for (y = j * resize; y < j * resize + resize; y++) {
					sum_red 	+= in->red[x][y];
					if (out->type == 6) {
						sum_green 	+= in->green[x][y];
						sum_blue 	+= in->blue[x][y];
					}
				}
			}
			// este calculata valoarea noului pixel ca fiind media aritmetica
			// a celor vechi
			out->red[i][j] = sum_red / o;
			if (out->type == 6) {
				out->green[i][j] = sum_green / o;
				out->blue[i][j] = sum_blue / o;
			}
		}
	}
}

/*
* Functia "micsoreaza" imaginea primita de un numar impar de ori 
* (3) prin calcularea sumei pixelilor din matrice de 3x3 inmultiti
* element cu element cu matricea Gaussiana si impartita la suma 
* elementelor celei din urma.
*/
void resize_even(image *in, image * out) {
	int i, j, x, y, sum_red, sum_green, sum_blue;
	int resize = resize_factor;
	// matricea gaussiana
	int GaussKernel[3][3] = {{1,2,1}, {2,4,2}, {1,2,1}};

	#pragma omp parallel for 	\
		private(i,j,x,y,sum_red, sum_green, sum_blue)  \
		num_threads(num_threads) 	\
		schedule(dynamic, CHUNK)

	// sunt parcursi pixelii imaginii noi
	for (i = 0; i < out->height; i++) {
		for( j = 0; j < out->width; j++) {
			sum_red = 0;
			sum_green = 0; 
			sum_blue = 0;  
			// este calculata suma pixel-ilor vechi din matrici de 3x3 
			// inmultiti cu valorile corespunzatoare din matricea Gaussiana
			for (x = i*resize; x < i * resize + resize; x++) {
				for (y = j * resize; y < j * resize + resize; y++) {
					sum_red	+= in->red[x][y] * GaussKernel[x%3][y%3];
					if (out->type == 6) {
						sum_green += in->green[x][y] * GaussKernel[x%3][y%3];
						sum_blue += in->blue[x][y] * GaussKernel[x%3][y%3];
					}
				}
			}
			// este calculata valoarea noului pixel
			out->red[i][j] = sum_red / 16;
			if (out->type == 6) {
				out->green[i][j] = sum_green / 16;
				out->blue[i][j] = sum_blue / 16;
			}
		}
	}
}

/*
* Functia calculeaza noua dimensiune pentru noua imagine, 
* initiaza zona de memorie pentru aceasta si apeleaza functia
* ce va calcula valorile noilor pixeli
*/
void resize(image *in, image * out) { 

	// creare antet
	out->maxVal = in->maxVal;
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->type = in->type;
	// alocare memorie
	initImage(out);

	if (resize_factor % 2 == 0)
		resize_odd(in, out);
	else resize_even(in, out);

	freeImage(in);
}