#include <stdio.h>      // Ajoute ceci pour pouvoir utiliser perror
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>       // Inclure errno.h pour l'accès à errno et aux erreurs standards

#include "shm_const.h"

int creer_segment_memoire(key_t key, int *shmid) {

/* création du segment de mémoire partagée avec la clé CLE_SHM */
if ((*shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0660)) < 0) {
  perror("shmget");
  exit(1);
}

return(*shmid);
}


int * attacher_segment_memoire(int * mem, int *shmid) {
/* attachement */
if ((mem = shmat(*shmid, NULL, 0)) == (int *) -1) {
  perror("shmat");
  exit(1);
}
else {
  return(mem);
}

}