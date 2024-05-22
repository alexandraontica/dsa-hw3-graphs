/* ONȚICĂ Alexandra-Elena - 311CB */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GRAF_H
#define GRAF_H

#define INT_MAX 2147483647

typedef struct celarc { 
    char *destinatie;    // orasul destinatie al rutei
    int nr_costuri;      // numarul de tronsoane care compun ruta (task 1)/numar costuri == 1 (task 2)
    float *cost;         // vector grade afectare (task 1)/costul (task 2)
    int *modificat;      // -1/0/1 -> daca si cum a fost gradul de afectare modificat (task 1)
    int input;           // 0/1 -> arcul se afla sau nu in fisierul de input 
    int nr_ordine;       // a cata ruta este in fisierul de intrare
    struct celarc *urm;  // adresa urmatorul adiacent
} TCelArc, *AArc;

typedef struct {
    int n;        // numar noduri
    char **src;   // vector etichete noduri (orasele de unde pornesc muchiile)
    AArc* v;      // vector adrese liste arce
} TGL;

// pentru afisare la cerinta 2:
typedef struct {
    char *start;
    char *destinatie;
} TMuchie;

void CloseFiles(FILE *fin, FILE *fout);
TGL* AlocG();
AArc AlocArc(int nr, char *dest);
int AdaugaOras(TGL *g, char *oras);
void InserareRutaSf(AArc *lista, AArc ruta);
int CautaNod(TGL *g, char* oras);
int AdaugaRuta(int cerinta, FILE *fin, TGL *g, int ord);
void SortareRute(AArc *rute, char **orase, int R);
void ModificaGradAfectare(TGL *g, int K);
void PastreazaRute(FILE *fout, AArc *rute, int L, int R);
int MinNode(TGL *g, int *visited, int *distances);
int* Dijkstra(TGL *g, int n, char **last);
void NumarParcurgeri(int nodCurent, char **last, int *nrDrMin, TGL *g);
TMuchie* SalveazaMuchiiDrumuriMinime(TGL *graf, char **last);
void Swap(int i, int j, int *nrDrMin, TMuchie *muchii, int *distante, int *ordine, char **last);
void SortareMuchiiDesc(TMuchie *muchii, int *nrDrMin, int *distante, int *ordine, char **last, int nr);
void SortareMuchiiCresc(TMuchie *muchii, char **last, int *ordine, int nr);
void DistrArc(AArc arc);
void DistrG(TGL** ag);
void DistrLast(char ***last, int nr);

#endif  // GRAF_H
