#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h> 
#include "shm_const.h"
#include <time.h>



// Fonction pour afficher uniquement la somme des morts
void afficher_somme_morts(Armee *armee) {
    int morts_totaux = 0;

    //Initialiser l'heure
    time_t t = time(NULL);
    struct tm *current_time = localtime(&t);

    // Itération sur toutes les compagnies pour collecter les pertes en morts
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                morts_totaux += armee->divisions[i].regiments[j].compagnies[k].pertes.morts;
                printf("À %02d:%02d Pertes de la compagnie C%d du régiment R%d de la division D%d \n",current_time->tm_hour, current_time->tm_min, k, j, i);
                printf("\t Pertes : %d\n", armee->divisions[i].regiments[j].compagnies[k].pertes.morts);
                printf("\t Blessés : %d\n", armee->divisions[i].regiments[j].compagnies[k].pertes.blesses);
                printf("\t Ennemis tués : %d\n", armee->divisions[i].regiments[j].compagnies[k].pertes.ennemis_morts);
                printf("\t Prisonniers : %d\n", armee->divisions[i].regiments[j].compagnies[k].pertes.prisonniers);



            }
        }
    }

    printf("Total des morts dans l'armée : %d\n", morts_totaux);
}

// Fonction pour lancer chaque compagnie
void lancer_compagnie(int index) {
    pid_t pid = fork();  // Créer un processus enfant pour chaque compagnie
    if (pid == 0) {
        // Processus enfant : ici on peut exécuter le code de la compagnie
        char index_str[10];
        snprintf(index_str, sizeof(index_str), "%d", index);

        // On exécute ici un programme qui représente la compagnie
        execlp("./compagnie", "compagnie", index_str, NULL);
        
        // Si execlp échoue
        perror("Erreur lors du lancement de la compagnie");
        exit(1);
    }
    else if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
}

// Fonction principale
int main() {
    // Créer la mémoire partagée pour l'armée
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

    // Lancer les processus pour chaque compagnie (N_DIVISIONS * N_REGIMENTS * N_COMPAGNIES compagnies)
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                // Chaque compagnie est un processus enfant
                lancer_compagnie(i * N_REGIMENTS * N_COMPAGNIES + j * N_COMPAGNIES + k);
            }
        }
    }

    // Attendre que tous les processus enfants (compagnies) se terminent
    for (int i = 0; i < N_DIVISIONS * N_REGIMENTS * N_COMPAGNIES; i++) {
        wait(NULL);
    }

    // Afficher la somme des morts toutes les 10 secondes
    while (1) {
        afficher_somme_morts(armee);
        sleep(10);  // Attendre 10 secondes avant de réafficher
    }

    // Détacher la mémoire partagée avant de quitter
    shmdt(armee);
    return 0;
}
