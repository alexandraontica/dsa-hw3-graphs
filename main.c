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
        // nu exista niciun parametru in linia de comanda
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
            // citesc si adaug ruta curenta in graf:
            int rez = AdaugaRuta(1, fin, graf, i);
            if (!rez) {
                DistrG(&graf);
                CloseFiles(fin, fout);
                return 0;
            }
        }

        // modific gradele de afectare in K ani:
        ModificaGradAfectare(graf, K);

        // creez un vector de rute concomitent cu un vector de orase:
        // (vectorul de orase retine orasul de start al fiecarei rute)
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

        // sortez rutele dupa numarul de ordine ca sa le afisez
        // in ordinea in fisierul de intrare:
        SortareRute(rute, orase, R);

        // afisare rute:
        for (i = 0; i < R; i++) {
            fprintf(fout, "%s %s ", orase[i], rute[i]->destinatie);
            fprintf(fout, "%d ", rute[i]->nr_costuri);
                
            int j;
            for (j = 0; j < rute[i]->nr_costuri; j++) {
                fprintf(fout, "%.2f ", rute[i]->cost[j]);
            }
            fprintf(fout, "\n");
        }

        // determin si afisez indicii rutelor care pot fi pastrate:
        PastreazaRute(fout, rute, L, R);

        // eliberez memoria alocata:
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
            // citesc si adaug ruta curenta in graf:
            int rez = AdaugaRuta(2, fin, graf, i);
            if (!rez) {
                DistrG(&graf);
                CloseFiles(fin, fout);
                return 0;
            }
        }

        // determin pozitia nodului de start in graf:
        int nodStart = CautaNod(graf, start); 

        char **last = (char **)calloc(graf->n, sizeof(char *));
        // last retine ultimul oras prin care se trece inainte sa se
        // ajunga la orasul dorit, parcurgand drumul de cost minim
        if (!last) {
            DistrG(&graf);
            CloseFiles(fin, fout);
            return 0;
        }

        // calculez distantele minime de la nodul de start la fiecare nod din graf:
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

        // numar in nrDrMin de cate ori se trece prin fiecare muchie cand se
        // parcurg toate drumurile minime:
        for (i = 0; i < graf->n; i++) {
            NumarParcurgeri(i, last, nrDrMin, graf);
        }

        // creez un vector cu muchiile parcurse in cadrul drumurilor minime:
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
        // vectorul ordine retine numarul de ordine al muchiilor in fisierul de input

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

        // sorteaza muchiile descrescator dupa numarul de parcurgeri in cadrul drumurilor minime
        SortareMuchiiDesc(muchii, nrDrMin, distante, ordine, last, graf->n);

        // numar cate muchii sunt parcurse in cadrul drumurilor minime:
        int nrRute = 0;
        for (i = 0; i < graf->n; i++) {
            if (last[i]) {
                nrRute++;
            }
        }
        
        // determin cate rute/muchii voi afisa si afisez acest numar:
        if (nrRute < K) {
            K = nrRute;
            fprintf(fout, "%d\n", K);
        } else {
            fprintf(fout, "%d\n", K);
        }

        // sortez muchiile crescator dupa numarul de ordine
        // ca sa le afisez in ordinea in care apar in input:
        SortareMuchiiCresc(muchii, last, ordine, K);

        // afisez muchiile:
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
