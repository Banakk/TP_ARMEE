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

    // Ajouter les pertes des compagnies au régiment
    for (int i = 0; i < N_COMPAGNIES; i++) {
        division->regiments[regiment_id].compagnies[i].pertes.morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.blesses += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.ennemis_morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.prisonniers += rand() % 10;
    }

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
