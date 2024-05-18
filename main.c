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
            int rez = AdaugaRuta(fin, graf);
            if (!rez) {
                DistrG(&graf);
                CloseFiles(fin, fout);
            }
        }

        ModificaGradAfectare(graf, K);
        Afisare1(fout, graf);
        PastreazaRute(fout, graf, L);

        DistrG(&graf);
    } else if (!strcmp(argv[1], "2")) {
        // cerinta 2
        char start[20];
        
    } else {
        fprintf(stderr, "Cerinta invalida\n");
    }

    CloseFiles(fin, fout);
    return 0;
}
