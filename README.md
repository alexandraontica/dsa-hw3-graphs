*Onțică Alexandra-Elena - 311CB*

# Tema 3 - Grafuri

## Constructie graf
Am alocat un pointer de tip TGL, i-am setat numarul de noduri la 0 si campurile vectori (cel de etichete - src - si cel de adrese liste arce - v) la NULL. Pe acestea le realoc de fiecare data cand adaug un nod nou. Am citit numarul de rute si fiecare ruta in parte. Pentru fiecare ruta am verificat daca exista deja in graf orasele de start si destinatie. Daca nu, le-am adaugat in graf (am creat nod/uri nou/noi). Am alocat apoi cate un arc pentru fiecare ruta si i-am populat campurile cu informatiile corespunzatoare. Am adaugat apoi noile arce la finalul listelor ce corespund oraselor de start ale fiecarei rute.

De mentionat e faptul ca fiecare ruta am adaugat-o de doua ori in graf, o data dus, o data intors, adica o data ca arc incident nodului de start si o data ca arc incident cu nodul destinatie. Pentru ruta dus (adica cea citita din fisier) am salvat in campul `input` 1 (ca sa afisez la final doar rutele citite, nu si pe cele de intoarcere, destinatie-start). La rutele de intoarcere am salvat vectorul de costuri inversat.

## Cerinta 1
Am construit graful pe baza informatiilor din fisierul de intrare. 

Pentru fiecare an, am modificat gradele de afectare ale tronsoanelor astfel:

Am initializat pentru fiecare arc toate pozitiile din campul `modificat` cu 0. Am parcurs fiecare tronson in parte. Daca acesta avea gradul de afectare diferit de 0, am verificat daca ii pot dubla valoarea astfel incat sa nu depaseasca 100. In caz afirmativ, am dublat valoarea, altfel gradul de afectare devine 100. 

Daca gradul de afectare este 0, am identificat mai multe cazuri posibile. Daca tronsonul nu se afla la marginea rutei, am calculat maximul dintre vecinii din stanga si dreapta, verificand daca gradul lor de afectare a fost deja modificat si cum. Daca acesta a fost deja dublat, compar max cu jumatate din gradul curent. Daca inainte sa fie modificat a avut valoarea 0, valoarea maximului nu se modifica cu nimic. 
Daca exista un singur tronson pe ruta, vecinii sunt primele trosoane de pe muchiile incidente cu nodul de start si cu nodul destinatie. Calcuze la fel ca mai devreme maximul dintre acesti vecini.
Daca tronsonul este primul din ruta, un vecin se afla pe aceeasi ruta, in dreapta sa, iar restul vecinilor sunt primele tronsoane de pe muchiile incidente cu nodul de start.
Daca transonul este ultimul in ruta, un vecin se afla pe aceeasi ruta, in stanga sa, iar restul vecinilor sunt primele tronsoane de pe muchiile incidente nodului destinatie.
In toate cazurile, noua valoare va fi jumatate din maxim.

Ca sa pot afisa rutele cu gradele de afectare modificate in ordinea din fisierul de intrare am creat un vector de arce pe care l-am sortat crescator dupa campul `nr_ordine` si pe care l-am afisat. 

Ca sa determin rutele care vor fi pastrate, am parcurs fiecare arc in parte, am calculat media aritmetica a gradelor de afectare ale tronsoanelor arcului curent si, daca, media era mai mica decat gradul de uzura acceptabil, am afisat indicele rutei.

La final am eliberat memoria alocata.

## Cerinta 2
Am construit graful pe baza informatiilor din fisierul de intrare. 

Am determinat pozitia in graf a orasului de start. Mi-am alocat un vector de string-uri `last` in care va fi salvat pentru fiecare nod din graf ultimul oras prin care se trece inainte sa se ajunga la nodul curent, parcurgand drumul de cost minim. Am determinat distantele minime de la nodul de start la fiecare nod din graf folosind algoritmul Dijkstra (am folosit implementarea de la laborator, adaugand in plus popularea vectorului `last`). Am numarat de cate ori se trece prin fiecare muchie cand se parcurg toate drumurile minime foloindu-ma de o functie recursiva (pt fiecare nod parcurge drumul minim in sens invers si marcheaza muchiile pe care le parcurge). Muchiile care sunt parcurse de cele mai multe ori trebuie pastrate. Am creat un vector cu muchiile ce formeaza drumurile minime si am sortat acest vector descrescator dupa numarul de parcurgeri. Am calculat cate muchii trebuie sa pastrez (numarul total de muchii parcurse in drumurile minime daca acesta este mai mic decat K, K muchii altfel) si am pastrat in K valoarea obtinuta. Ca sa afisez muchiile in ordinea din fisierul de intrare, am sortat crescator primele K elemente din vectorul de muchii dupa numarul de ordine si le-am afisat.

La final am eliberat memoria alocata.
