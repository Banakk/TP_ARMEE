#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include "shm_const.h"
#include <time.h>

void afficher_somme_morts(Armee *armee) {
    int morts_totaux = 0;  // Initialiser le compteur de morts totaux  
    int blesses_totaux = 0 ;
    int ennemis_mort_totaux = 0 ;
    int prisonnier_totaux = 0;

    time_t t = time(NULL);
    struct tm *current_time = localtime(&t);

    for (int i = 0; i < N_DIVISIONS; i++) {
        printf("=== Division D%d ===\n", i);

        for (int j = 0; j < N_REGIMENTS; j++) {
            printf("  === Régiment R%d ===\n", j); 

            for (int k = 0; k < N_COMPAGNIES; k++) {
                printf("    === Compagnie C%d ===\n", k);

                int morts = armee->divisions[i].regiments[j].compagnies[k].pertes.morts;
                int blesses = armee->divisions[i].regiments[j].compagnies[k].pertes.blesses;
                int ennemis_morts = armee->divisions[i].regiments[j].compagnies[k].pertes.ennemis_morts;
                int prisonniers = armee->divisions[i].regiments[j].compagnies[k].pertes.prisonniers;

                printf("À %02d:%02d Pertes de la compagnie C%d du régiment R%d de la division D%d\n",
                       current_time->tm_hour, current_time->tm_min, k, j, i);
                printf("\t Pertes : %d\n", morts);
                printf("\t Blessés : %d\n", blesses);
                printf("\t Ennemis tués : %d\n", ennemis_morts);
                printf("\t Prisonniers : %d\n", prisonniers);

                morts_totaux += morts;
                blesses_totaux += blesses;
                ennemis_mort_totaux += ennemis_morts;
                prisonnier_totaux += prisonniers;
            }
        }
    }

    printf("Total des morts dans l'armée : %d\n", morts_totaux);
    printf("Total des ennemies morts : %d\n", ennemis_mort_totaux);
    printf("Total des blessés dans l'armée : %d\n", blesses_totaux);
    printf("Total des prisonniers pour l'armée : %d\n", prisonnier_totaux);

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
