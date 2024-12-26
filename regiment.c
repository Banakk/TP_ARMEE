#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "sem_op.h"
#include <unistd.h>
#include <time.h>

#define SLEEP_DURATION 10

// Fonction pour traiter les résultats d'un régiment donné
void traiter_resultats_regiment(int semid, Division *division, int regiment_id) {
    P(semid);  // Attente du sémaphore

    // Boucle pour accumuler des pertes pour chaque compagnie du régiment
    for (int i = 0; i < N_COMPAGNIES; i++) {
        division->regiments[regiment_id].compagnies[i].pertes.morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.blesses += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.ennemis_morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.prisonniers += rand() % 10;
    }

    V(semid);  // Libération du sémaphore
}

// Fonction pour vérifier les erreurs de shmget et shmat
void verifier_memoire_partagee(int shm_id, Armee *armee) {
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(EXIT_FAILURE);
    }

    if (armee == (void *)-1) {
        perror("Erreur de shmat");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour détacher correctement la mémoire partagée
void detacher_memoire_partagee(Armee *armee) {
    if (shmdt(armee) == -1) {
        perror("Erreur de shmdt");
        exit(EXIT_FAILURE);
    }
}

// Fonction principale
int main() {
    srand(time(NULL));  // Initialisation de la graine aléatoire pour rand()

    // Création de la mémoire partagée pour l'armée
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), IPC_CREAT | 0666);
    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);

    // Vérification des erreurs de mémoire partagée
    verifier_memoire_partagee(shm_id, armee);

    // Création et initialisation des sémaphores
    int semid;
    creer_initialiser_semaphore(CLE_SEM, &semid);

    // Traitement des résultats pour chaque régiment de la première division
    for (int i = 0; i < N_REGIMENTS; i++) {
        traiter_resultats_regiment(semid, &armee->divisions[0], i);
        sleep(SLEEP_DURATION);  // Pause de 10 secondes entre chaque traitement de régiment
    }

    // Détacher la mémoire partagée
    detacher_memoire_partagee(armee);

    return 0;
}
