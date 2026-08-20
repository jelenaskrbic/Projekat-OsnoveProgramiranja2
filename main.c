#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void kreirajGrafIFajl(int brojLokacija, double procenatGustine, const char* imeFajla) {
    long long maksimumPuteva = ((long long)brojLokacija * (brojLokacija - 1)) / 2;
    long long potrebanBrojPuteva = (long long)(procenatGustine * maksimumPuteva);

    printf("Generisanje grafa (%d cvorova, %lld grana). Ovo moze potrajati...\n", brojLokacija, potrebanBrojPuteva);


    char** postojiGrana = (char**)malloc(brojLokacija * sizeof(char*));
    for (int i = 0; i < brojLokacija; i++) {
        postojiGrana[i] = (char*)calloc(brojLokacija, sizeof(char));
    }

    FILE* fajl = fopen(imeFajla, "w");
    if (!fajl) {
        printf("Greska pri kreiranju fajla %s!\n", imeFajla);
        return;
    }

    fprintf(fajl, "%d %lld\n", brojLokacija, potrebanBrojPuteva);

    long long kreirano = 0;
    while (kreirano < potrebanBrojPuteva) {
        int r1 = rand() % brojLokacija;
        int r2 = rand() % brojLokacija;


        if (r1 != r2 && postojiGrana[r1][r2] == 0) {
            postojiGrana[r1][r2] = 1;
            postojiGrana[r2][r1] = 1;

            int tezina = (rand() % 100) + 1;
            fprintf(fajl, "%d %d %d\n", r1, r2, tezina);
            kreirano++;
        }
    }

    fclose(fajl);


    for (int i = 0; i < brojLokacija; i++) {
        free(postojiGrana[i]);
    }
    free(postojiGrana);
    printf("Uspjesno sacuvano u fajl: %s\n\n", imeFajla);
}

int main() {
    srand(time(NULL));
    int opcija = -1;

    while (opcija != 0) {
        printf("=== GENERATOR GRAFOVA ===\n");
        printf("1. Generisi 800 cvorova (30%% gustine)\n");
        printf("2. Generisi 3000 cvorova (60%% gustine)\n");
        printf("3. Generisi 7000 cvorova (80%% gustine) - UPOZORENJE: Velik fajl (~300MB)\n");
        printf("0. Izlaz\n");
        printf("Izbor: ");
        scanf("%d", &opcija);

        switch (opcija) {
            case 1: kreirajGrafIFajl(800, 0.3, "graf_800_30.txt"); break;
            case 2: kreirajGrafIFajl(3000, 0.6, "graf_3000_60.txt"); break;
            case 3: kreirajGrafIFajl(7000, 0.8, "graf_7000_80.txt"); break;
            case 0: printf("Kraj rada generatora.\n"); break;
            default: printf("Nepostojeca opcija!\n\n");
        }
    }
    return 0;
}
