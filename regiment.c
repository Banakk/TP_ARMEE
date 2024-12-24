#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "sem_op.h"
#include <time.h>  // Pour initialiser la fonction rand()

// Function to process the results of the regiment
void traiter_resultats_regiment(int semid, Division *division, int regiment_id) {
    P(semid);  // Wait on the semaphore

    // Add losses to the companies in the regiment
    for (int i = 0; i < N_COMPAGNIES; i++) {
        division->regiments[regiment_id].compagnies[i].pertes.morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.blesses += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.ennemis_morts += rand() % 10;
        division->regiments[regiment_id].compagnies[i].pertes.prisonniers += rand() % 10;
    }

    V(semid);  // Release the semaphore
}

int main() {
    // Initialize the random number generator
    srand(time(NULL));

    // Attach to the shared memory
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

    // Initialize the semaphore for the regiment
    int semid;
    creer_initialiser_semaphore(CLE_SEM, &semid);

    // Process the results of the regiment
    for (int i = 0; i < N_REGIMENTS; i++) {
        traiter_resultats_regiment(semid, &armee->divisions[0], i);
    }

    // Detach from the shared memory
    if (shmdt(armee) == -1) {
        perror("Erreur de shmdt");
        exit(1);
    }

    return 0;
}
