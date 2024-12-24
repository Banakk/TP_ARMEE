#include <stdio.h>      // Ajoute ceci pour pouvoir utiliser perror
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>       // Inclure errno.h pour l'accès à errno et aux erreurs standards

#include "shm_const.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "shm_const.h"

int creer_segment_memoire(key_t key, int *shmid) {
    // Création du segment de mémoire partagée avec la clé CLE_SHM
    if ((*shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0660)) < 0) {
        perror("shmget");
        exit(1);
    }
    return *shmid;
}

int* attacher_segment_memoire(int *shmid) {
    // Attachement du segment de mémoire partagée
    int* mem = (int*) shmat(*shmid, NULL, 0);
    if (mem == (int*) -1) {
        perror("shmat");
        exit(1);
    }
    return mem;  // Renvoie un pointeur vers la mémoire partagée
}
