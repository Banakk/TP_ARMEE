#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "sem_op.h"
#include <time.h>
#include <unistd.h>

#define SLEEP_DURATION 10

// Fonction pour traiter les résultats d'une division donnée
void traiter_resultats_division(int semid, Armee *armee, int division_id) {
    P(semid);  // Attente du sémaphore
    
    // Boucle à travers les régiments et compagnies pour générer des pertes aléatoires
    for (int i = 0; i < N_REGIMENTS; i++) {
        for (int j = 0; j < N_COMPAGNIES; j++) {
            // Accumuler des pertes aléatoires pour chaque compagnie
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.morts += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.blesses += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.ennemis_morts += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.prisonniers += rand() % 10;
        }
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

// Fonction pour traiter et détacher correctement la mémoire partagée
void detacher_memoire_partagee(Armee *armee) {
    if (shmdt(armee) == -1) {
        perror("Erreur de shmdt");
        exit(EXIT_FAILURE);
    }
}

// Fonction principale
int main() {
    int semid;
    int shm_id;

    // Création et initialisation de la mémoire partagée
    shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), IPC_CREAT | 0666);
    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);
    
    // Vérification des erreurs pour shmget et shmat
    verifier_memoire_partagee(shm_id, armee);

    // Création et initialisation des sémaphores
    creer_initialiser_semaphore(CLE_SEM, &semid);
    
    // Traitement des résultats pour chaque division
    for (int i = 0; i < N_DIVISIONS; i++) {
        traiter_resultats_division(semid, armee, i);
        sleep(SLEEP_DURATION);  // Pause de 10 secondes entre chaque traitement des divisions
    }
    
    // Détacher la mémoire partagée
    detacher_memoire_partagee(armee);

    return 0;
}
