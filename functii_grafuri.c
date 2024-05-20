/* ONȚICĂ Alevandra-Elena - 311CB */
#include "functii_grafuri.h"

void CloseFiles(FILE *fin, FILE *fout)
// inchide fisierele de input si output
{
    fclose(fin);
    fclose(fout);
}

TGL* AlocG()
{
    TGL *g = (TGL *)malloc(sizeof(TGL));
    if (!g) {
        return NULL;
    }

    g->n = 0;

    // atribui vectorilor valoarea NULL
    // ii voi realoca cand voi adauga elemente
    g->src = NULL;
    g->v = NULL;

    return g;
}

AArc AlocArc(int nr, char *dest)
{
    AArc arc = (AArc)calloc(1, sizeof(TCelArc));
    if(!arc) {
        return NULL;
    }

    arc->destinatie = strdup(dest);
    if (!arc->destinatie) {
        free(arc);
        return NULL;
    }

    arc->nr_costuri = nr;
    arc->urm = NULL;

    arc->cost = (float *)calloc(nr, sizeof(float));
    if (!arc->cost) {
        free(arc->destinatie);
        free(arc);
        return NULL;
    }

    arc->modificat = (int *)calloc(nr, sizeof(int));
    if (!arc->modificat) {
        free(arc->destinatie);
        free(arc->cost);
        free(arc);
        return NULL;
    }

    arc->input = 0;
    // input retine daca muchia face parte din fisierul de input
    // de ex, daca citesc Londra-Paris, pentru muchia cu startul in Londra si 
    // destinatia in Parit input o sa fie 1;
    // pentru aceeasi muchie, dar adaugata in lista de adiacenta a nodului Paris,
    // (startul in Paris si destinatia in Londra), input o sa fie 0;
    // modific valoarea lui input in afara functiei de alocare

    return arc;
}

int AdaugaOras(TGL *g, char *oras)
// adauga un oras/nod nou in graf
{
    char **aux1 = (char **)realloc(g->src, (g->n + 1) * sizeof(char *));
    if (!aux1) {
        return 0;
    }
    g->src = aux1;

    g->src[g->n] = strdup(oras);
    if (!g->src[g->n]) {
        return 0;
    }

    AArc *aux2 = (AArc *)realloc(g->v, (g->n + 1) * sizeof(AArc));
    if (!aux2) {
        free(g->src[g->n]);
        return 0;
    }
    g->v = aux2;

    g->v[g->n] = NULL;
    g->n++;

    return 1;
}

void InserareRutaSf(AArc *lista, AArc ruta)
// insereaza ruta/arcul la finalul listei
{
    AArc p = *lista;
    AArc ultim = NULL;

    while (p) {
        ultim = p;
        p = p->urm;
    }

    if (ultim) {
        ultim->urm = ruta;
    } else {
        *lista = ruta;
    }
}

int AdaugaRuta(int cerinta, FILE *fin, TGL *g)
// returneaza 1 daca ruta a fost adaugata cu succes, 0 in caz contrar
{
    char start[20], dest[20];
    int nr;  // numar tronsoane
    fscanf(fin, "%s", start);
    fscanf(fin, "%s", dest);

    if (cerinta == 1) {
        fscanf(fin, "%d", &nr);
    } else {
        nr = 1;
    }

    int i, poz_start = -1, poz_dest = -1;
    for (i = 0; i < g->n; i++) {
        // verific daca exista orasul de start in graf:
        if(!strcmp(start, g->src[i])) {
            poz_start = i;
        }

        // verific daca exista orasul destinatie in graf:
        if(!strcmp(dest, g->src[i])) {
            poz_dest = i;
        }
    }
    
    if (poz_start == -1) {
        // orasul de start nu exista in graf
        // il adaug:
        int rez = AdaugaOras(g, start);
        if (!rez) {
            return 0;
        }
        poz_start = g->n - 1;
    }

    if (poz_dest == -1) {
        // orasul destinatie nu exista in graf
        // il adaug:
        int rez = AdaugaOras(g, dest);
        if (!rez) {
            return 0;
        }
        poz_dest = g->n - 1;
    }

    // adauga ruta in graf:
    // (dus-intors, caci este graf neorientat)

    // alocare ruta dus:
    AArc ruta_dus = AlocArc(nr, dest);
    if (!ruta_dus) {
        return 0;
    }
    ruta_dus->input = 1;

    // alocare ruta intors:
    AArc ruta_intors = AlocArc(nr, start);
    if (!ruta_dus) {
        DistrArc(ruta_dus);
        return 0;
    }

    // populeaza vectorii cu gradele de afectare de pe fiecare tronson:
    for (i = 0; i < nr; i++) {
        float procent;
        fscanf(fin, "%f", &procent);

        ruta_dus->cost[i] = procent;
        ruta_intors->cost[nr - i - 1] = procent;
    }

    // legare ruta dus:
    InserareRutaSf(&g->v[poz_start], ruta_dus);

    // legare ruta intors:
    InserareRutaSf(&g->v[poz_dest], ruta_intors);

    return 1;
}

void ModificaGradAfectare(TGL *g, int K)
{
    // de fiecare data cand modific un tronson, 
    // marchez acest lucru in campul modificat (vector) al arcului 

    // legenda camp modificat[j]:
    // daca valoarea este 0, nu a fost modificat, 
    // daca este 1, valoarea i-a fost deja dublata, 
    // daca este -1, era 0 inainte si valoarea i-a fost actualizata

    int q;
    for (q = 0; q < K; q++) {
        int i;

        for (i = 0; i < g->n; i++) {
            AArc l = g->v[i];
            while (l) {
                int j;
                // initializare camp "modificat" pentru anul curent
                for (j = 0; j < l->nr_costuri; j++) {
                    l->modificat[j] = 0;
                }
                l = l->urm;
            }
        }

        for (i = 0; i < g->n; i++) {
            AArc l = g->v[i];
            while (l) {
                int j;
                for (j = 0; j < l->nr_costuri; j++) {
                    if (l->cost[j]) {
                        // daca gradul de afectare este diferit de 0
                        // verific daca il pot dubla astfel incat
                        // sa nu depaseasca 100
                        if (l->cost[j] * 2 < 100) {
                            l->cost[j] = l->cost[j] * 2;
                        } else {
                            l->cost[j] = 100;
                        }
                        l->modificat[j] = 1;
                    } else {
                        // daca gradul de afectare este 0
                        if (j > 0 && j < l->nr_costuri - 1) {
                            float max = 0;
                            if (l->modificat[j - 1] == 1) {
                                max = l->cost[j - 1] / 2;
                            } else if (l->modificat[j - 1] == 0) {
                                max = l->cost[j - 1];
                            }

                            if (l->modificat[j + 1] == 1) {
                                if (max < l->cost[j + 1] / 2) {
                                    max = l->cost[j + 1] / 2;
                                }
                            } else if (l->modificat[j + 1] != -1) {
                                if (max < l->cost[j + 1]) {
                                    max = l->cost[j + 1];
                                }
                            }
                            l->cost[j] = max / 2;
                            l->modificat[j] = -1;
                        } else if (j == 0) {
                            float max = 0;

                            // daca vectorul de costuri nu are un singur element:
                            if (j + 1 < l->nr_costuri) {
                                if (l->modificat[j + 1] == 0) {
                                    max = l->cost[j + 1];
                                } else if (l->modificat[j + 1] == 1) {
                                    max = l->cost[j + 1] / 2;
                                }
                            }

                            AArc p = g->v[i];
                            for (; p != NULL; p = p->urm) {
                                if (p->modificat[0] == 1 && p->cost[0] / 2 > max) {
                                    max = p->cost[0] / 2;
                                } else if (p->modificat[0] == 0 && p->cost[0] > max) {
                                    max = p->cost[0];
                                }
                            }
                            l->cost[j] = max / 2;
                            l->modificat[j] = -1;
                        } else if (j == l->nr_costuri - 1) {
                            float max = 0;
                            if (l->modificat[j - 1] == 0) {
                                max = l->cost[j - 1];
                            } else if (l->modificat[j - 1] == 1) {
                                max = l->cost[j - 1] / 2;
                            }

                            int t;
                            for (t = 0; t < g->n; t++) {
                                if (!strcmp(g->src[t], l->destinatie)) {
                                    break;
                                }
                            }

                            AArc p = g->v[t];
                            for (; p != NULL; p = p->urm) {
                                if (p->modificat[0] == 1 && p->cost[0] / 2 > max) {
                                    max = p->cost[0] / 2;
                                } else if (p->modificat[0] == 0 && p->cost[0] > max) {
                                    max = p->cost[0];
                                }
                            }
                            l->cost[j] = max / 2;
                            l->modificat[j] = -1;
                        }
                    }
                }
                l = l->urm;
            }
        }
    }
}

void Afisare1(FILE *fout, TGL *g)
// afisare graf - cerinta 1
{
    int i;
    for (i = 0; i < g->n; i++) {
        AArc l = g->v[i];
        while (l) {
            if (l->input) {
                fprintf(fout, "%s %s ", g->src[i], l->destinatie);
                fprintf(fout, "%d ", l->nr_costuri);
                
                int j;
                for (j = 0; j < l->nr_costuri; j++) {
                    fprintf(fout, "%.2f ", l->cost[j]);
                }
                fprintf(fout, "\n");
            }
            l = l->urm;
        }
    }
}

void PastreazaRute(FILE *fout, TGL *g, int L)
// determina rutele care merita pastrate si le afiseaza indicii
{
    int idx = 0;  // retine indexul rutei curente
    int i;
    for (i = 0; i < g->n; i++) {
        AArc l = g->v[i];
        while (l) {
            if (l->input) {
                idx++;

                float suma = 0;
                int j;
                for (j = 0; j < l->nr_costuri; j++) {
                    suma = suma + l->cost[j];
                }

                if (suma / l->nr_costuri <= L) {
                    fprintf(fout, "%d ", idx);
                }
            }
            l = l->urm;
        }
    }
}

// pentru cerinta 2 am folosit implementarile facute la laborator:
int MinNode(TGL *g, int *visited, int *distances)
{
    int minDist = INT_MAX;
    int result;
    for (int i = 0; i < g->n; i++) {
        if (visited[i] == 0 && distances[i] < minDist) {
            result = i;
            minDist = distances[i];
        }
    }
    return result;
}

int* Prim(TGL *g, int n) 
{
    // aloc spatiu pt visited, distances, parents
    int *visited, *distances, *parents;
    visited = (int *)calloc(g->n, sizeof(int));
    distances = (int *)calloc(g->n, sizeof(int));
    parents = (int *)calloc(g->n, sizeof(int));

    // initializari vectori (parents -> 0 -> calloc)
    for (int  i = 0; i < g->n; i++) {
        distances[i] = INT_MAX;
    }

    distances[n] = 0;
    parents[n] = -1;  // o valoare care nu exista in graf = valoarea radacinii arborelui de acoperire minim

    for (int i = 0; i < g->n; i++) {
        // selectare nod de vizitat: nodul sa nu fie vizitat si sa aiba distanta minima
        int index = MinNode(g, visited, distances);
        visited[index] = 1;

        // parcurgem vecinii nodului pe care doar ce l-am vizitat
        for(AArc L = g->v[index]; L != NULL; L = L->urm) {
            int j;
            for (j = 0; j < g->n; j++) {
                if (!strcmp(L->destinatie, g->src[j])) {
                    break;
                }
            } 

            if ((int)L->cost[0] < distances[j] && visited[j] == 0) {
                distances[j] = (int)L->cost[0];
                parents[j] = index;
            }
        }
    }

    free(distances);
    free(visited);

    return parents;
}

void DistrArc(AArc arc)
// elibereaza memoria alocata pentru o muchie
{
    free(arc->destinatie);
    free(arc->modificat);
    free(arc->cost);
    free(arc);
}

void DistrG(TGL **ag)
// elibereaza memoria alocata pentru graf
{
    int i;
    AArc p, aux;
    for (i = 0; i < (*ag)->n; i++) {
        free((*ag)->src[i]);
        p = (*ag)->v[i];
        while (p) {
            aux = p;
            p = p->urm;
            DistrArc(aux);
        }
    }
    free((*ag)->src);
    free((*ag)->v);
    free(*ag);
    *ag = NULL;
}
