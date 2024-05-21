/* ONȚICĂ Alexandra-Elena - 311CB */
#include "functii_grafuri.h"

int main(int argc, char *argv[])
{
    // deschid fisierele de input/output:
    FILE *fin = fopen("tema3.in", "r");

    if (fin == NULL) {
        fprintf(stderr, "Fisierul de intrare nu a fost deschis\n");
        return 0;
    }

    FILE *fout = fopen("tema3.out", "w");

    if (fout == NULL) {
        fprintf(stderr, "Fisierul de iesire nu a fost deschis\n");
        fclose(fin);
        return 0;
    }
    
    if (argc == 1) {
        printf("Rulati din nou, specificand numarul cerintei de rezolvat!\n");
        CloseFiles(fin, fout);
        return 0;
    }

    // identific cerinta pe care trebuie sa o rezolv:
    if (!strcmp(argv[1], "1")) {
        // cerinta 1

        TGL *graf = AlocG();
        if (!graf) {
            CloseFiles(fin, fout);
            return 0;
        }

        int R, K, L;
        fscanf(fin, "%d%d%d", &R, &K, &L);

        int i;
        for (i = 0; i < R; i++) {
            int rez = AdaugaRuta(1, fin, graf, i);
            if (!rez) {
                DistrG(&graf);
                CloseFiles(fin, fout);
                return 0;
            }
        }

        ModificaGradAfectare(graf, K);

        AArc *rute = (AArc *)calloc(R, sizeof(AArc));
        if (!rute) {
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        char **orase = (char **)calloc(R, sizeof(char *));
        if (!orase) {
            free(rute);
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        int idx = 0;
        for (i = 0; i < graf->n; i++) {
            AArc l = graf->v[i];
            while (l) {
                if (l->input) {
                    rute[idx] = l;
                    orase[idx] = strdup(graf->src[i]);
                    if (!orase[idx]) {
                        int t = 0;
                        for (t = 0; t < idx; t++) {
                            free(orase[t]);
                        }
                        free(orase);
                        free(rute);
                        DistrG(&graf);
                        CloseFiles(fin, fout);
                        return 0;
                    }

                    idx++;
                }
                l = l->urm;
            }
        }

        SortareRute(rute, orase, R);

        for (i = 0; i < R; i++) {
            fprintf(fout, "%s %s ", orase[i], rute[i]->destinatie);
            fprintf(fout, "%d ", rute[i]->nr_costuri);
                
            int j;
            for (j = 0; j < rute[i]->nr_costuri; j++) {
                fprintf(fout, "%.2f ", rute[i]->cost[j]);
            }
            fprintf(fout, "\n");
        }

        PastreazaRute(fout, rute, L, R);

        for(i = 0; i < R; i++) {
            free(orase[i]);
        }
        free(orase);

        free(rute);
        DistrG(&graf);
    } else if (!strcmp(argv[1], "2")) {
        // cerinta 2
        TGL *graf = AlocG();
        if (!graf) {
            CloseFiles(fin, fout);
            return 0;
        }

        char start[21];  // presupun ca nu am nume de orase mai lungi de 20 caractere
        fscanf(fin, "%s", start);

        int K, M;
        fscanf(fin, "%d%d", &K, &M);

        int i;
        for (i = 0; i < M; i++) {
            int rez = AdaugaRuta(2, fin, graf, i);
            if (!rez) {
                DistrG(&graf);
                CloseFiles(fin, fout);
                return 0;
            }
        }

        int nodStart = CautaNod(graf, start); 

        char **last = (char **)calloc(graf->n, sizeof(char *));
        // last retine ultimul oras prin care se trece inainte sa se
        // ajunga la orasul dorit, parcurgand drumul de cost minim
        if (!last) {
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        int *distante = Dijkstra(graf, nodStart, last);
        if (!distante) {
            DistrLast(&last, graf->n);
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        int *nrDrMin = (int *)calloc(graf->n, sizeof(int));
        if (!nrDrMin) {
            free(distante);
            DistrLast(&last, graf->n);
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        for (i = 0; i < graf->n; i++) {
            NumarParcurgeri(i, last, nrDrMin, graf);
        }

        TMuchie *muchii = SalveazaMuchiiDrumuriMinime(graf, last);
        if (!muchii) {
            free(nrDrMin);
            free(distante);
            DistrLast(&last, graf->n);
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        int *ordine = (int *)calloc(graf->n, sizeof(int));
        if (!ordine) {
            free(muchii);
            free(nrDrMin);
            free(distante);
            DistrLast(&last, graf->n);
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        for (i = 0; i < graf->n; i++) {
            if (last[i]) {
                AArc l;
                for (l = graf->v[i]; l; l = l->urm) {
                    if (!strcmp(last[i], l->destinatie)) {
                        break;
                    }
                }
                ordine[i] = l->nr_ordine;
            }
        }

        SortareMuchiiDesc(muchii, nrDrMin, distante, ordine, last, graf->n);

        int nrRute = 0;
        for (i = 0; i < graf->n; i++) {
            if (last[i]) {
                nrRute++;
            }
        }
        
        if (nrRute < K) {
            K = nrRute;
            fprintf(fout, "%d\n", K);
        } else {
            fprintf(fout, "%d\n", K);
        }

        SortareMuchiiCresc(muchii, last, ordine, K);

        for (i = 0; i < K; i++) {
            if (last[i]) {
                fprintf(fout, "%s %s\n", muchii[i].start, muchii[i].destinatie);
            }
        }

        // eliberez memoria alocata:
        for (i = 0; i < graf->n; i++) {
            free(muchii[i].start);
            free(muchii[i].destinatie);
        }
        free(muchii);

        free(ordine);
        free(nrDrMin);
        free(distante);
        DistrLast(&last, graf->n);
        DistrG(&graf);
    } else {
        fprintf(stderr, "Cerinta invalida\n");
    }

    CloseFiles(fin, fout);
    return 0;
}
