#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm_const.h"
#include "aleatoire.h"
#include <time.h>

// Fonction pour générer les pertes pour une compagnie
void generer_pertes(Compagnie *compagnie) {
    compagnie->pertes.morts = generer_perte();
    compagnie->pertes.blesses = generer_perte();
    compagnie->pertes.ennemis_morts = generer_ennemis_morts();
    compagnie->pertes.prisonniers = generer_prisonniers();
}

// Fonction pour vérifier et gérer les erreurs liées à la mémoire partagée
void verifier_memoire_partagee(Armee *armee, int shm_id) {
    if (armee == (void *)-1) {
        perror("Erreur de shmat");
        exit(EXIT_FAILURE);
    }
    if (shm_id == -1) {
        perror("Erreur de shmget");
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <index_compagnie>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Récupération de l'index de la compagnie depuis les arguments
    int index = atoi(argv[1]);
    if (index < 0 || index >= N_DIVISIONS * N_REGIMENTS * N_COMPAGNIES) {
        fprintf(stderr, "Index invalide : %d\n", index);
        exit(EXIT_FAILURE);
    }

    // Calcul des indices pour la division, le régiment et la compagnie
    int division_index = index / (N_REGIMENTS * N_COMPAGNIES);
    int regiment_index = (index % (N_REGIMENTS * N_COMPAGNIES)) / N_COMPAGNIES;
    int company_index = index % N_COMPAGNIES;

    // Accès à la mémoire partagée
    int shm_id = shmget(CLE_MEMOIRE, sizeof(Armee), 0666);
    Armee *armee = (Armee *)shmat(shm_id, NULL, 0);

    // Vérification des erreurs liées à la mémoire partagée
    verifier_memoire_partagee(armee, shm_id);

    // Initialisation de la génération aléatoire
    init_random();

    // Génération des pertes pour la compagnie ciblée
    generer_pertes(&armee->divisions[division_index].regiments[regiment_index].compagnies[company_index]);

    // Détachement de la mémoire partagée
    detacher_memoire_partagee(armee);

    return 0;
}
