#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "sem_op.h"
#include <time.h>  // Pour initialiser la fonction rand()

// Fonction pour traiter les résultats du régiment
void traiter_resultats_regiment(int semid, Division *division, int regiment_id) {
    P(semid);  // Attente sur le sémaphore

    // Variables pour totaliser les pertes au niveau du régiment
    int total_morts = 0;
    int total_blesses = 0;
    int total_ennemis_morts = 0;
    int total_prisonniers = 0;

    // Ajouter les pertes des compagnies au régiment
    for (int i = 0; i < N_COMPAGNIES; i++) {
        // Calcul des pertes pour chaque compagnie
        int morts = rand() % 10;
        int blesses = rand() % 10;
        int ennemis_morts = rand() % 10;
        int prisonniers = rand() % 10;

        // Remplir les informations des pertes pour chaque compagnie
        division->regiments[regiment_id].compagnies[i].pertes.morts += morts;
        division->regiments[regiment_id].compagnies[i].pertes.blesses += blesses;
        division->regiments[regiment_id].compagnies[i].pertes.ennemis_morts += ennemis_morts;
        division->regiments[regiment_id].compagnies[i].pertes.prisonniers += prisonniers;

        // Totaliser les pertes au niveau du régiment
        total_morts += morts;
        total_blesses += blesses;
        total_ennemis_morts += ennemis_morts;
        total_prisonniers += prisonniers;
    }

    // Afficher le total des pertes pour ce régiment
    printf("Régiment %d :\n", regiment_id);
    printf("Total morts : %d\n", total_morts);
    printf("Total blessés : %d\n", total_blesses);
    printf("Total ennemis morts : %d\n", total_ennemis_morts);
    printf("Total prisonniers : %d\n\n", total_prisonniers);

    V(semid);  // Libération du sémaphore
}

int main() {
    // Initialiser la fonction rand() pour garantir des nombres aléatoires différents à chaque exécution
    srand(time(NULL));

    // Créer la mémoire partagée pour la division
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Division), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(1);
    }

    // Attacher la mémoire partagée à l'espace mémoire du processus
    Division *division = (Division *)shmat(shm_id, NULL, 0);
    if (division == (void *)-1) {
        perror("Erreur de shmat");
        exit(1);
    }

    // Initialiser le sémaphore pour le régiment
    int semid;
    creer_initialiser_semaphore(CLE_SEM, &semid);

    // Traitement des résultats du régiment
    for (int i = 0; i < N_REGIMENTS; i++) {
        traiter_resultats_regiment(semid, division, i);
    }

    // Détacher la mémoire partagée après utilisation
    if (shmdt(division) == -1) {
        perror("Erreur de shmdt");
        exit(1);
    }

    return 0;
}
