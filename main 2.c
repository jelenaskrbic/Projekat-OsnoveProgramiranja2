#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEDOSTIZNO 99999999
#define MAX_TRANZITA 8

struct Konekcija {
    int ciljniCvor;
    int distanca;
    struct Konekcija* sljedeci;
};

struct Graf {
    int ukupanBrojLokacija;
    struct Konekcija** listaPuteva;
};

void ubaciPut(struct Graf* m, int cvorA, int cvorB, int distanca) {
    struct Konekcija* novaVeza1 = (struct Konekcija*)malloc(sizeof(struct Konekcija));
    novaVeza1->ciljniCvor = cvorB; novaVeza1->distanca = distanca;
    novaVeza1->sljedeci = m->listaPuteva[cvorA];
    m->listaPuteva[cvorA] = novaVeza1;

    struct Konekcija* novaVeza2 = (struct Konekcija*)malloc(sizeof(struct Konekcija));
    novaVeza2->ciljniCvor = cvorA; novaVeza2->distanca = distanca;
    novaVeza2->sljedeci = m->listaPuteva[cvorB];
    m->listaPuteva[cvorB] = novaVeza2;
}

struct Graf* ucitajIzFajla(const char* imeFajla) {
    FILE* fajl = fopen(imeFajla, "r");
    if (!fajl) {
        printf("Greska: Ne mogu otvoriti %s!\n", imeFajla);
        return NULL;
    }

    int brCvorova;
    long long brGrama;
    fscanf(fajl, "%d %lld", &brCvorova, &brGrama);

    struct Graf* m = (struct Graf*)malloc(sizeof(struct Graf));
    m->ukupanBrojLokacija = brCvorova;
    m->listaPuteva = (struct Konekcija**)malloc(brCvorova * sizeof(struct Konekcija*));
    for (int i = 0; i < brCvorova; i++) m->listaPuteva[i] = NULL;

    printf("Ucitavanje grafa iz fajla... ");
    for (long long i = 0; i < brGrama; i++) {
        int u, v, w;
        fscanf(fajl, "%d %d %d", &u, &v, &w);
        ubaciPut(m, u, v, w);
    }
    fclose(fajl);
    printf("Zavrseno!\n");
    return m;
}

void stampajPutanjuRekurzivno(int* prethodnik, int cvor, int preskociPrvi) {
    if (prethodnik[cvor] != -1) {
        stampajPutanjuRekurzivno(prethodnik, prethodnik[cvor], preskociPrvi);
        printf(" -> %d", cvor);
    } else {
        if (!preskociPrvi) printf("%d", cvor);
    }
}

void dijkstra(struct Graf* m, int start, int* udaljenosti, int* prethodnici) {
    int N = m->ukupanBrojLokacija;
    int* obradjen = (int*)calloc(N, sizeof(int));

    for (int j = 0; j < N; j++) {
        udaljenosti[j] = NEDOSTIZNO;
        prethodnici[j] = -1;
    }
    udaljenosti[start] = 0;

    for (int j = 0; j < N - 1; j++) {
        int najkrace = NEDOSTIZNO;
        int minIndeks = -1;
        for (int k = 0; k < N; k++) {
            if (!obradjen[k] && udaljenosti[k] < najkrace) {
                najkrace = udaljenosti[k];
                minIndeks = k;
            }
        }
        if (minIndeks == -1) break;
        obradjen[minIndeks] = 1;

        struct Konekcija* komsija = m->listaPuteva[minIndeks];
        while (komsija != NULL) {
            int v = komsija->ciljniCvor;
            int w = komsija->distanca;
            if (!obradjen[v] && udaljenosti[minIndeks] + w < udaljenosti[v]) {
                udaljenosti[v] = udaljenosti[minIndeks] + w;
                prethodnici[v] = minIndeks;
            }
            komsija = komsija->sljedeci;
        }
    }
    free(obradjen);
}


void obradiPermutaciju(int* trenutnaPerm, int brojU, int* cvoroviTSP, int** udaljenostiTSP, int* minDist, int* najboljaPerm) {
    int ukupno = 0;
    int trenIndex = 0;

    for (int i = 0; i < brojU; i++) {
        int idSljedeci = trenutnaPerm[i];
        int dist = udaljenostiTSP[trenIndex][cvoroviTSP[idSljedeci]];
        if (dist == NEDOSTIZNO) return;
        ukupno += dist;
        trenIndex = idSljedeci;
    }


    int distDoCilja = udaljenostiTSP[trenIndex][cvoroviTSP[brojU + 1]];
    if (distDoCilja == NEDOSTIZNO) return;
    ukupno += distDoCilja;

    if (ukupno < *minDist) {
        *minDist = ukupno;
        for (int i = 0; i < brojU; i++) najboljaPerm[i] = trenutnaPerm[i];
    }
}

void generisiPermutacije(int* arr, int l, int r, int brojU, int* cvoroviTSP, int** udaljenostiTSP, int* minDist, int* najboljaPerm) {
    if (l == r) {
        obradiPermutaciju(arr, brojU, cvoroviTSP, udaljenostiTSP, minDist, najboljaPerm);
    } else {
        for (int i = l; i <= r; i++) {
            int temp = arr[l]; arr[l] = arr[i]; arr[i] = temp;
            generisiPermutacije(arr, l + 1, r, brojU, cvoroviTSP, udaljenostiTSP, minDist, najboljaPerm);
            temp = arr[l]; arr[l] = arr[i]; arr[i] = temp;
        }
    }
}

void optimalnaRutaSaStanicama(struct Graf* m, int start, int cilj, int* tranziti, int brojU) {
    int N_TSP = brojU + 2;
    int* cvoroviTSP = (int*)malloc(N_TSP * sizeof(int));
    cvoroviTSP[0] = start;
    for (int i = 0; i < brojU; i++) cvoroviTSP[i + 1] = tranziti[i];
    cvoroviTSP[brojU + 1] = cilj;


    int** udaljenostiTSP = (int**)malloc((brojU + 1) * sizeof(int*));
    int** prethodniciTSP = (int**)malloc((brojU + 1) * sizeof(int*));

    printf("Analiziram puteve izmedju zadanih cvorova...\n");
    for (int i = 0; i <= brojU; i++) {
        udaljenostiTSP[i] = (int*)malloc(m->ukupanBrojLokacija * sizeof(int));
        prethodniciTSP[i] = (int*)malloc(m->ukupanBrojLokacija * sizeof(int));
        dijkstra(m, cvoroviTSP[i], udaljenostiTSP[i], prethodniciTSP[i]);
    }

    int* trenutnaPerm = (int*)malloc(brojU * sizeof(int));
    int* najboljaPerm = (int*)malloc(brojU * sizeof(int));
    for (int i = 0; i < brojU; i++) trenutnaPerm[i] = i + 1;

    int minDist = NEDOSTIZNO;
    generisiPermutacije(trenutnaPerm, 0, brojU - 1, brojU, cvoroviTSP, udaljenostiTSP, &minDist, najboljaPerm);

    if (minDist == NEDOSTIZNO) {
        printf("\nNije moguce pronaci put koji obilazi sve zadane stanice!\n");
    } else {
        printf("\n=== NAJKRACA MOGUCA RUTA PRONADJENA ===\n");
        printf("Tok kretanja: ");

        int trenIndex = 0;
        for (int i = 0; i < brojU; i++) {
            int sljedeciIndex = najboljaPerm[i];
            int preskociPrvi = (i > 0) ? 1 : 0;
            stampajPutanjuRekurzivno(prethodniciTSP[trenIndex], cvoroviTSP[sljedeciIndex], preskociPrvi);
            trenIndex = sljedeciIndex;
        }
        int preskociPrvi = (brojU > 0) ? 1 : 0;
        stampajPutanjuRekurzivno(prethodniciTSP[trenIndex], cvoroviTSP[brojU + 1], preskociPrvi);

        printf("\nUkupna duzina najkrace rute: %d\n", minDist);
        printf("=======================================\n");
    }


    for (int i = 0; i <= brojU; i++) {
        free(udaljenostiTSP[i]);
        free(prethodniciTSP[i]);
    }
    free(udaljenostiTSP); free(prethodniciTSP);
    free(cvoroviTSP); free(trenutnaPerm); free(najboljaPerm);
}

int main() {
    char imeFajla[50];
    printf("Unesite ime fajla sa grafom (npr. graf_800_30.txt): ");
    scanf("%s", imeFajla);

    struct Graf* Graf = ucitajIzFajla(imeFajla);
    if (!Graf) return 1;

    int st, ci, brojU;
    printf("Unesite STARTNI cvor (od 0 do %d): ", Graf->ukupanBrojLokacija - 1);
    scanf("%d", &st);

    printf("Unesite CILJNI cvor (od 0 do %d): ", Graf->ukupanBrojLokacija - 1);
    scanf("%d", &ci);

    printf("Unesite broj OBAVEZNIH USPUTNIH STANICA (gradova koje morate posjetiti, max %d): ", MAX_TRANZITA);
    scanf("%d", &brojU);

    if (brojU > MAX_TRANZITA) {
        printf("Greska: Zbog performansi (O(n!)), maksimum stanica je %d.\n", MAX_TRANZITA);
        return 1;
    }

    if (brojU > 0) {
        int* tranziti = (int*)malloc(brojU * sizeof(int));
        for (int j = 0; j < brojU; j++) {

            printf("Unesite ID cvora za usputnu stanicu %d (od 0 do %d): ", j + 1, Graf->ukupanBrojLokacija - 1);
            scanf("%d", &tranziti[j]);
        }
        optimalnaRutaSaStanicama(Graf, st, ci, tranziti, brojU);
        free(tranziti);
    } else {
        int* udaljenosti = (int*)malloc(Graf->ukupanBrojLokacija * sizeof(int));
        int* prethodnici = (int*)malloc(Graf->ukupanBrojLokacija * sizeof(int));
        dijkstra(Graf, st, udaljenosti, prethodnici);

        if (udaljenosti[ci] != NEDOSTIZNO) {
            printf("\n=== DETALJI IZRACUNATE RUTE ===\nTok kretanja: ");
            stampajPutanjuRekurzivno(prethodnici, ci, 0);
            printf("\nUkupna duzina puta: %d\n===============================\n", udaljenosti[ci]);
        } else {
            printf("Put nije pronadjen.\n");
        }
        free(udaljenosti); free(prethodnici);
    }

    return 0;
}
