/* ONȚICĂ Alevandra-Elena - 311CB */
#include "functii_grafuri.h"

void CloseFiles(FILE *fin, FILE *fout)
// inchide fisierele de input si output
{
    fclose(fin);
    fclose(fout);
}

TGL* AlocG()
// aloca memorie pentru graf
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
// aloca memorie pentru un arc/o ruta
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
    // input retine daca muchia face parte din fisierul de input:
    // de ex, daca citesc Londra-Paris, pentru muchia cu startul in Londra si 
    // destinatia in Parit input o sa fie 1;
    // pentru aceeasi muchie, dar adaugata in lista de adiacenta a nodului Paris,
    // (startul in Paris si destinatia in Londra), input o sa fie 0;
    // modific valoarea lui input in afara functiei de alocare

    arc->nr_ordine = -1;
    // modific valoarea lui nr_ordine in afara functiei de alocare

    return arc;
}

int AdaugaOras(TGL *g, char *oras)
// adauga un oras/nod nou in graf
{
    // ii aloc spatiu in campurile vectori din structura:
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

    // determin ultima celula din lista:
    while (p) {
        ultim = p;
        p = p->urm;
    }

    if (ultim) {
        ultim->urm = ruta;
    } else {
        // daca lista este vida
        *lista = ruta;
    }
}

int CautaNod(TGL *g, char* oras)
// cauta un oras in graf
// returneaza pozitia sa daca l-a gasit sau -1 in caz contrar
{
    int i;
    for (i = 0; i < g->n; i++) {
        if (!strcmp(oras, g->src[i])) {
            return i;
        }
    }

    return -1;
}

int AdaugaRuta(int cerinta, FILE *fin, TGL *g, int ord)
// citeste informatiile rutei curente, creeaza ruta si o adauga in graf
// returneaza 1 daca ruta a fost adaugata cu succes, 0 in caz contrar
{
    char start[20], dest[20];
    int nr;  // numar tronsoane
    fscanf(fin, "%s", start);
    fscanf(fin, "%s", dest);

    if (cerinta == 1) {
        fscanf(fin, "%d", &nr);
    } else {
        // cerinta 2 are un singur cost pe fiecare muchie
        nr = 1;
    }

    // caut orasele citite in graf:
    int poz_start = CautaNod(g, start);
    int poz_dest = CautaNod(g, dest);
    
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
    int i;
    for (i = 0; i < nr; i++) {
        float procent;
        fscanf(fin, "%f", &procent);

        ruta_dus->cost[i] = procent;
        ruta_intors->cost[nr - i - 1] = procent;
    }

    // salvez pozitia rutei in fisierul de citire
    // pentru a le putea afisa la final in ordinea corecta:
    ruta_dus->nr_ordine = ord;
    ruta_intors->nr_ordine = ord;

    // legare ruta dus:
    InserareRutaSf(&g->v[poz_start], ruta_dus);

    // legare ruta intors:
    InserareRutaSf(&g->v[poz_dest], ruta_intors);

    return 1;
}

void ModificaGradAfectare(TGL *g, int K)
// modifica gradele de afectare pe parcursul a K ani
{
    // de fiecare data cand modific un tronson, 
    // marchez acest lucru in campul modificat (vector) al arcului 

    // legenda camp modificat[j]:
    // daca valoarea este 0, tronsonul nu a fost modificat, 
    // daca este 1, valoarea i-a fost deja dublata, 
    // daca este -1, era 0 inainte si valoarea i-a fost actualizata

    int q;
    for (q = 0; q < K; q++) {
        int i;
        for (i = 0; i < g->n; i++) {
            AArc l = g->v[i];
            while (l) {
                // initializare camp "modificat" pentru anul curent:
                int j;
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
                        // daca gradul de afectare este diferit de 0,
                        // verific daca il pot dubla astfel incat
                        // sa nu depaseasca 100:
                        if (l->cost[j] * 2 < 100) {
                            l->cost[j] = l->cost[j] * 2;
                        } else {
                            l->cost[j] = 100;
                        }
                        l->modificat[j] = 1;
                    } else {
                        // daca gradul de afectare este 0,
                        // determin maximul dintre vecini,
                        // tinand cont de valorile din campul modificat,
                        // pentru a afla noua valoare:
                        if (j > 0 && j < l->nr_costuri - 1) {
                            // daca nu se afla "la margine",
                            // vecinii se afla pe aceeasi ruta
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
                        } else if (l->nr_costuri == 1) {
                            // daca exista un singur tronson pe ruta,
                            // vecinii sunt primele trosoane de pe muchiile
                            // incidente cu nodul de start si cu nodul destinatie
                            float max = 0;
                            AArc p = g->v[i];
                            for (; p != NULL; p = p->urm) {
                                if (p->modificat[0] == 1 && p->cost[0] / 2 > max) {
                                    max = p->cost[0] / 2;
                                } else if (p->modificat[0] == 0 && p->cost[0] > max) {
                                    max = p->cost[0];
                                }
                            }

                            int t = CautaNod(g, l->destinatie);

                            p = g->v[t];
                            for (; p != NULL; p = p->urm) {
                                if (p->modificat[0] == 1 && p->cost[0] / 2 > max) {
                                    max = p->cost[0] / 2;
                                } else if (p->modificat[0] == 0 && p->cost[0] > max) {
                                    max = p->cost[0];
                                }
                            }
                            l->cost[j] = max / 2;
                            l->modificat[j] = -1;
                        } else if (j == 0) {
                            // transonul este primul in ruta;
                            // vecinul din dreapta se afla pe aceeasi ruta,
                            // restul vecinilor sunt primele tronsoane de pe
                            // muchiile incidente nodului de start
                            float max = 0;
                            if (l->modificat[j + 1] == 0) {
                                max = l->cost[j + 1];
                            } else if (l->modificat[j + 1] == 1) {
                                max = l->cost[j + 1] / 2;
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
                            // transonul este ultimul in ruta;
                            // vecinul din stanga se afla pe aceeasi ruta,
                            // restul vecinilor sunt primele tronsoane de pe
                            // muchiile incidente nodului destinatie
                            float max = 0;
                            if (l->modificat[j - 1] == 0) {
                                max = l->cost[j - 1];
                            } else if (l->modificat[j - 1] == 1) {
                                max = l->cost[j - 1] / 2;
                            }

                            int t = CautaNod(g, l->destinatie);
                            // destinatia sigur exista in graf (din constructia grafului),
                            // deci t este sigur diferit de -1

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

void SortareRute(AArc *rute, char **orase, int R)
// sorteaza rutele crescator dupa numarul de ordine
// ca sa le afisez in ordinea in care apar in input
{
    int i, j;
    for(i = 0; i < R - 1; i++) {
        for (j = i + 1; j < R; j++) {
            if (rute[i]->nr_ordine > rute[j]->nr_ordine) {
                AArc aux1 = rute[i];
                rute[i] = rute[j];
                rute[j] = aux1;

                char *aux2 = orase[i];
                orase[i] = orase[j];
                orase[j] = aux2;
            }
        }
    }
}

void PastreazaRute(FILE *fout, AArc *rute, int L, int R)
// determina rutele care merita pastrate si le afiseaza indicii
{
    int i;
    for (i = 0; i < R; i++) {
        float suma = 0;
        int j;
        for (j = 0; j < rute[i]->nr_costuri; j++) {
            suma = suma + rute[i]->cost[j];
        }

        if (suma / rute[i]->nr_costuri < L) {
            fprintf(fout, "%d ", i + 1);  // rutele se numara de la 1, nu de la 0
        }
    }  
}

int MinNode(TGL *g, int *visited, int *distances)
// determina primul nod care nu e vizitat si are distanta minima
{
    int minDist = INT_MAX;
    int result, i;
    for (i = 0; i < g->n; i++) {
        if (visited[i] == 0 && distances[i] < minDist) {
            result = i;
            minDist = distances[i];
        }
    }

    return result;
}

int* Dijkstra(TGL *g, int n, char **last)
// determina distanta minima de la un nod dat la celelalte noduri din graf
{
    // aloc spatiu pt vectorii visited si distances
    int *visited = (int *)calloc(g->n , sizeof(int));
    if (!visited) {
        return NULL;
    }

    int *distances = (int *)calloc(g->n, sizeof(int));
    if (!distances) {
        free(visited);
        return NULL;
    }

    // initializare distante:
    int i;
    for (i = 0; i < g->n; i++) {
        distances[i] = INT_MAX;
    }
    distances[n] = 0;

    for (i = 0; i < g->n; i++) {
        // selectez nod de vizitat (nodul sa nu fie vizitat si sa aiba distanta minima):
        int index = MinNode(g, visited, distances);
        visited[index] = 1;

        // parcurg vecinii nodului pe care doar ce l-am vizitat:
        AArc L;
        for(L = g->v[index]; L != NULL; L = L->urm) {
            int index_dest = CautaNod(g, L->destinatie);

            if (distances[index] + (int)L->cost[0] < distances[index_dest] && 
                visited[index_dest] == 0) {
                // am gasit o distanta mai mica, modific valoarea din distances:
                distances[index_dest] = distances[index] + (int)L->cost[0];

                // modific si ultimul nod prin care se trece inainte sa se
                // ajunga la orasul dorit, parcurgand drumul de cost minim:
                free(last[index_dest]);
                last[index_dest] = strdup(g->src[index]);
                if (!last[index_dest]) {
                    free(visited);
                    free(distances);
                    return NULL;
                }
            }
        }
    }

    free(visited);
    return distances;
}

void NumarParcurgeri(int nodCurent, char **last, int *nrDrMin, TGL *g)
// marcheaza ce muchii se parcurg in drumul minim de la nodul de start la nodul curent
{
    if (!last[nodCurent]) {
        // am ajuns la orasul de start
        return;
    }

    nrDrMin[nodCurent]++;

    // determin pozitia nodului prin care am trecut inainte sa ajung la nodul curent:
    // (parcurg drumul minim invers, de la nodul curent la nodul de start)
    int nod = CautaNod(g, last[nodCurent]);

    NumarParcurgeri(nod, last, nrDrMin, g);
}

TMuchie* SalveazaMuchiiDrumuriMinime(TGL *graf, char **last)
// creeaza un vector cu toate muchiile parcurse in drumurile minime
{
    int i;
    TMuchie *muchii = (TMuchie *)calloc(graf->n, sizeof(TMuchie));
    if (!muchii) {
        return NULL;
    }

    for (i = 0; i < graf->n; i++) {
        if (last[i]) {
            int esteInput = 0;
            AArc l;
            for (l = graf->v[i]; l; l = l->urm) {
                if (!strcmp(last[i], l->destinatie)) {
                    if (l->input) {
                        esteInput = 1;
                    }
                    break;
                }
            }

            if (esteInput) {
                muchii[i].start = strdup(graf->src[i]);
                if (!muchii[i].start) {
                    free(muchii);
                    return NULL;
                }

                muchii[i].destinatie = strdup(last[i]);
                if (!muchii[i].destinatie) {
                    free(muchii[i].start);
                    free(muchii);
                    return NULL;
                }
            } else {
                muchii[i].start = strdup(last[i]);
                if (!muchii[i].start) {
                    free(muchii);
                    return NULL;
                }

                muchii[i].destinatie = strdup(graf->src[i]);
                if (!muchii[i].destinatie) {
                    free(muchii[i].start);
                    free(muchii);
                    return NULL;
                }
            }
        }
    }

    return muchii;
}

void SortareMuchiiDesc(TMuchie *muchii, int *nrDrMin, int *distante, int *ordine, char **last, int nr)
// sorteaza muchiile descrescator dupa numarul de parcurgeri in cadrul drumurilor minime
// daca numarul de parcurgeri este egal, sorteaza muchiile crescator dupa distantele drumurilor minime
{
    int i, j;
    for (i = 0; i < nr - 1; i++) {
        for (j = i + 1; j < nr; j++) {
            if (nrDrMin[i] < nrDrMin[j]) {
                int aux1 = nrDrMin[i];
                nrDrMin[i] = nrDrMin[j];
                nrDrMin[j] = aux1;

                TMuchie aux2 = muchii[i];
                muchii[i] = muchii[j];
                muchii[j] = aux2;

                aux1 = distante[i];
                distante[i] = distante[j];
                distante[j] = aux1;

                aux1 = ordine[i];
                ordine[i] = ordine[j];
                ordine[j] = aux1;

                char *aux3 = last[i];
                last[i] = last[j];
                last[j] = aux3;
            } else if (nrDrMin[i] == nrDrMin[j]) {
                if (distante[i] > distante[j]) {
                    int aux1 = nrDrMin[i];
                    nrDrMin[i] = nrDrMin[j];
                    nrDrMin[j] = aux1;

                    TMuchie aux2 = muchii[i];
                    muchii[i] = muchii[j];
                    muchii[j] = aux2;

                    aux1 = distante[i];
                    distante[i] = distante[j];
                    distante[j] = aux1;

                    aux1 = ordine[i];
                    ordine[i] = ordine[j];
                    ordine[j] = aux1;

                    char *aux3 = last[i];
                    last[i] = last[j];
                    last[j] = aux3;
                }
            }
        }
    }
}

void SortareMuchiiCresc(TMuchie *muchii, char **last, int *ordine, int nr)
// sorteaza muchiile crescator dupa numarul de ordine
// ca sa le afisez in ordinea in care apar in input
{
    int i, j;
    for (i = 0; i < nr - 1; i++) {
        for (j = i + 1; j < nr; j++) {
            if (ordine[i] > ordine[j]) {
                int aux1 = ordine[i];
                ordine[i] = ordine[j];
                ordine[j] = aux1;

                TMuchie aux2 = muchii[i];
                muchii[i] = muchii[j];
                muchii[j] = aux2;

                char *aux3 = last[i];
                last[i] = last[j];
                last[j] = aux3;
            }
        }
    }
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

void DistrLast(char ***last, int nr)
// elibereaza memoria alocata pentru last (cerinta 2)
{
    int i;
    for (i = 0; i < nr; i++) {
        if ((*last)[i]) {
            free((*last)[i]);
        }
    }
    free(*last);
    *last = NULL;
}
