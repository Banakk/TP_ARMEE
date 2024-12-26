#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include "shm_const.h"
#include <time.h>
#include <unistd.h>
#define DEBUG

void afficher_somme_morts(Armee *armee) {
    int morts_totaux = 0;  // Initialiser les compteurs totaux
    int blesses_totaux = 0;
    int ennemis_mort_totaux = 0;
    int prisonnier_totaux = 0;

    for (int i = 0; i < N_DIVISIONS; i++) {
#ifdef DEBUG
        printf("=== Division D%d ===\n", i);
#endif
        int morts_division = 0;
        int blesses_division = 0;
        int ennemis_mort_division = 0;
        int prisonnier_division = 0;

        for (int j = 0; j < N_REGIMENTS; j++) {
#ifdef DEBUG
            printf("  === Régiment R%d ===\n", j);
#endif
            int morts_regiment = 0;
            int blesses_regiment = 0;
            int ennemis_mort_regiment = 0;
            int prisonnier_regiment = 0;

            for (int k = 0; k < N_COMPAGNIES; k++) {
#ifdef DEBUG
                printf("    === Compagnie C%d ===\n", k);
#endif
                int morts = armee->divisions[i].regiments[j].compagnies[k].pertes.morts;
                int blesses = armee->divisions[i].regiments[j].compagnies[k].pertes.blesses;
                int ennemis_morts = armee->divisions[i].regiments[j].compagnies[k].pertes.ennemis_morts;
                int prisonniers = armee->divisions[i].regiments[j].compagnies[k].pertes.prisonniers;

#ifdef DEBUG
                time_t t = time(NULL);
                struct tm *current_time = localtime(&t);
                printf("À %02d:%02d Pertes de la compagnie C%d du régiment R%d de la division D%d\n",
                       current_time->tm_hour, current_time->tm_min, k, j, i);
                printf("\t Pertes : %d\n", morts);
                printf("\t Blessés : %d\n", blesses);
                printf("\t Ennemis tués : %d\n", ennemis_morts);
                printf("\t Prisonniers : %d\n", prisonniers);
#endif
                morts_regiment += morts;
                blesses_regiment += blesses;
                ennemis_mort_regiment += ennemis_morts;
                prisonnier_regiment += prisonniers;
            }

#ifdef DEBUG
            printf("  Total des pertes pour le régiment R%d de la division D%d\n", j, i);
            printf("\t Pertes : %d\n", morts_regiment);
            printf("\t Blessés : %d\n", blesses_regiment);
            printf("\t Ennemis tués : %d\n", ennemis_mort_regiment);
            printf("\t Prisonniers : %d\n", prisonnier_regiment);
#endif
            sleep(10);  // Attendre 10 secondes avant de passer au régiment suivant

            morts_division += morts_regiment;
            blesses_division += blesses_regiment;
            ennemis_mort_division += ennemis_mort_regiment;
            prisonnier_division += prisonnier_regiment;
        }

#ifdef DEBUG
        printf("Total des pertes pour la division D%d\n", i);
        printf("\t Pertes : %d\n", morts_division);
        printf("\t Blessés : %d\n", blesses_division);
        printf("\t Ennemis tués : %d\n", ennemis_mort_division);
        printf("\t Prisonniers : %d\n", prisonnier_division);
#endif
        sleep(10);  // Attendre 10 secondes avant de passer à la division suivante

        morts_totaux += morts_division;
        blesses_totaux += blesses_division;
        ennemis_mort_totaux += ennemis_mort_division;
        prisonnier_totaux += prisonnier_division;
    }

    printf("Total des morts dans l'armée :\n");
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            int morts_regiment = 0;
            for (int k = 0; k < N_COMPAGNIES; k++) {
                morts_regiment += armee->divisions[i].regiments[j].compagnies[k].pertes.morts;
            }
            printf("R%d : %d\n", i * N_REGIMENTS + j + 1, morts_regiment);
        }
    }

    printf("Total des morts dans les divisions :\n");
    for (int i = 0; i < N_DIVISIONS; i++) {
        int morts_division = 0;
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                morts_division += armee->divisions[i].regiments[j].compagnies[k].pertes.morts;
            }
        }
        printf("D%d : %d\n", i + 1, morts_division);
    }
}

void lancer_compagnie(int index) {
    pid_t pid = fork();
    if (pid == 0) {
        char index_str[10];
        snprintf(index_str, sizeof(index_str), "%d", index);
        execlp("./compagnie", "compagnie", index_str, NULL);
        perror("Erreur lors du lancement de la compagnie");
        exit(1);
    } else if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
}

int main() {
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(1);
    }
    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);
    if (armee == (void *)-1) {
        perror("Erreur de shmat");
        exit(1);
    }
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                armee->divisions[i].regiments[j].compagnies[k].pertes.morts = 0;
                armee->divisions[i].regiments[j].compagnies[k].pertes.blesses = 0;
                armee->divisions[i].regiments[j].compagnies[k].pertes.ennemis_morts = 0;
                armee->divisions[i].regiments[j].compagnies[k].pertes.prisonniers = 0;
            }
        }
    }
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                lancer_compagnie(i * N_REGIMENTS * N_COMPAGNIES + j * N_COMPAGNIES + k);
            }
        }
    }
    for (int i = 0; i < N_DIVISIONS * N_REGIMENTS * N_COMPAGNIES; i++) {
        wait(NULL);
    }
    afficher_somme_morts(armee);
    shmdt(armee);
    return 0;
}
