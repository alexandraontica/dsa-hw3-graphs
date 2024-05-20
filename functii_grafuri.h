/* ONȚICĂ Alexandra-Elena - 311CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GRAF_H
#define GRAF_H

#define INT_MAX 2147483647

typedef struct celarc { 
    char *destinatie;
    int nr_costuri;
    float *cost;
    int *modificat;
    int input;
    struct celarc *urm;
} TCelArc, *AArc;

typedef struct {
    int n;       // numar noduri
    char **src;  // vector etichete noduri (orasele de unde pornesc muchiile)
    AArc* v;     // vector adrese liste arce
} TGL;

void CloseFiles(FILE *fin, FILE *fout);
TGL* AlocG();
AArc AlocArc(int nr, char *dest);
int AdaugaOras(TGL *g, char *oras);
int AdaugaRuta(int cerinta, FILE *fin, TGL *g);
void ModificaGradAfectare(TGL *g, int K);
void Afisare1(FILE *fout, TGL *g);
void PastreazaRute(FILE *fout, TGL *g, int L);
int* Prim(TGL *g, int n);
void DistrArc(AArc arc);
void DistrG(TGL** ag);

#endif  // GRAF_H
