// compagnie.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "aleatoire.h"

void generer_pertes(Compagnie *compagnie) {
    compagnie->pertes.morts = generer_perte();
    compagnie->pertes.blesses = generer_perte();
    compagnie->pertes.ennemis_morts = generer_ennemis_morts();
    compagnie->pertes.prisonniers = generer_prisonniers();
}

int main(int argc, char *argv[]) {
    int index = atoi(argv[1]);

    // Créer ou attacher à la mémoire partagée de la compagnie
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Compagnie), 0666);
    if (shm_id == -1) {
        perror("Erreur de shmget");
        exit(1);
    }

    Compagnie *compagnie = (Compagnie *)shmat(shm_id, NULL, 0);
    if (compagnie == (void *)-1) {
        perror("Erreur de shmat");
        exit(1);
    }

    // Initialiser la génération aléatoire
    init_random();

    // Générer les pertes pour la compagnie
    generer_pertes(compagnie);

    // Afficher les pertes pour vérifier que ça fonctionne
    printf("Pertes de la compagnie %d:\n", index);
    printf("Morts : %d\n", compagnie->pertes.morts);
    printf("Blessés : %d\n", compagnie->pertes.blesses);
    printf("Ennemis morts : %d\n", compagnie->pertes.ennemis_morts);
    printf("Prisonniers : %d\n", compagnie->pertes.prisonniers);

    return 0;
}