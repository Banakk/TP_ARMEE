#ifndef SEM_OP_H
#define SEM_OP_H

// Déclaration des fonctions pour l'opération P (attente) et V (libération)
void P(int semid);
void V(int semid);

// Fonction pour créer et initialiser un sémaphore
void creer_initialiser_semaphore(key_t cle, int *semid);

#endif
