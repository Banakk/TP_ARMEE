#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "aleatoire.h"
#include <time.h>

void generer_pertes(Compagnie *compagnie) {
    compagnie->pertes.morts = generer_perte();
    compagnie->pertes.blesses = generer_perte();
    compagnie->pertes.ennemis_morts = generer_ennemis_morts();
    compagnie->pertes.prisonniers = generer_prisonniers();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <index>\n", argv[0]);
        exit(1);
    }

    int index = atoi(argv[1]);

    // Attacher à la mémoire partagée
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), 0666);
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(1);
    }

    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);
    if (armee == (void *)-1) {
        perror("Erreur de shmat");
        exit(1);
    }

    // Initialiser la génération aléatoire
    init_random();

    // Calculer les indices de la division, du régiment et de la compagnie
    int division_index = index / (N_REGIMENTS * N_COMPAGNIES);
    int regiment_index = (index % (N_REGIMENTS * N_COMPAGNIES)) / N_COMPAGNIES;
    int company_index = index % N_COMPAGNIES;

    // Générer les pertes pour la compagnie
    generer_pertes(&armee->divisions[division_index].regiments[regiment_index].compagnies[company_index]);

    // Afficher les pertes pour cette compagnie
    time_t t = time(NULL);
    struct tm *current_time = localtime(&t);
    printf("À %02d:%02d Pertes de la compagnie C%d du régiment R%d de la division D%d\n",
           current_time->tm_hour, current_time->tm_min, company_index, regiment_index, division_index);
    printf("\tMorts : %d\n", armee->divisions[division_index].regiments[regiment_index].compagnies[company_index].pertes.morts);
    printf("\tBlessés : %d\n", armee->divisions[division_index].regiments[regiment_index].compagnies[company_index].pertes.blesses);
    printf("\tEnnemis morts : %d\n", armee->divisions[division_index].regiments[regiment_index].compagnies[company_index].pertes.ennemis_morts);
    printf("\tPrisonniers : %d\n", armee->divisions[division_index].regiments[regiment_index].compagnies[company_index].pertes.prisonniers);

    // Détacher de la mémoire partagée
    if (shmdt(armee) == -1) {
        perror("Erreur de shmdt");
        exit(1);
    }

    return 0;
}
