#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>  // Inclure errno.h pour l'accès à errno et aux erreurs standards

#include "shm_const.h"

static struct sembuf protection_shm;

void P(int semid) {
    protection_shm.sem_num = 0;
    protection_shm.sem_op = -1;
    protection_shm.sem_flg = 0;
    if (semop(semid, &protection_shm, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void V(int semid) {
    protection_shm.sem_num = 0;
    protection_shm.sem_op = 1;
    protection_shm.sem_flg = 0;
    if (semop(semid, &protection_shm, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

int creer_initialiser_semaphore(key_t cle, int * semid) {
    /* création du sémaphore */
    if ((*semid = semget(cle, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        if (errno == EEXIST) {
            /* Sémaphores déjà créé on ne fait qu'obtenir le semid */
            if ((*semid = semget(CLE_SEM, 0, 0600)) == -1) {
                perror("déjà créé mais semget");
                return -1;
            }
        } else {
            perror("semget");
            return -1;
        }
    }

    /* initialisation du sémaphore */
    if (semctl(*semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        return -1;
    }

    return 0;
}
