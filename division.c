#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "sem_op.h"
#include <time.h>  // Pour initialiser la fonction rand()

// Fonction pour traiter les résultats de la division et les envoyer à l'armée
void traiter_resultats_division(int semid, Armee *armee, int division_id) {
    // Attendre que tous les régiments de la division aient envoyé leurs résultats
    P(semid);  // Semaphores - attendre (P) avant de modifier la mémoire partagée
    
    // Ajouter des pertes aléatoires pour chaque compagnie
    for (int i = 0; i < N_REGIMENTS; i++) {
        for (int j = 0; j < N_COMPAGNIES; j++) {
            // Incrémenter les pertes dans la mémoire partagée pour l'armée
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.morts += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.blesses += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.ennemis_morts += rand() % 10;
            armee->divisions[division_id].regiments[i].compagnies[j].pertes.prisonniers += rand() % 10;
        }
    }

    // Libérer le sémaphore pour permettre aux autres processus d'entrer
    V(semid);  // Semaphores - libérer (V) après avoir modifié la mémoire partagée
}

int main() {
    // Initialiser la fonction rand()
    srand(time(NULL));  // Initialisation du générateur de nombres aléatoires
    
    // Créer la mémoire partagée pour l'armée
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(1);
    }

    // Attacher la mémoire partagée à l'espace mémoire de notre processus
    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);
    if (armee == (void *)-1) {
        perror("Erreur de shmat");
        exit(1);
    }

    // Initialiser le sémaphore pour la division
    int semid;
    creer_initialiser_semaphore(CLE_SEM, &semid);

    // Traitement des résultats de la division
    for (int i = 0; i < N_DIVISIONS; i++) {
        traiter_resultats_division(semid, armee, i);
    }

    // Détacher la mémoire partagée après utilisation
    if (shmdt(armee) == -1) {
        perror("Erreur de shmdt");
        exit(1);
    }

    return 0;
}