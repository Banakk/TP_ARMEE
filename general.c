#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "shm_const.h"  // Inclure shm_const.h qui contient la définition de Perte
#include <time.h>
#include <string.h>

#define DEBUG

// Fonction pour accumuler les pertes (en utilisant le bon type)
void accumuler_pertes(Perte *total, Perte *individuelles) {
    total->morts += individuelles->morts;
    total->blesses += individuelles->blesses;
    total->ennemis_morts += individuelles->ennemis_morts;
    total->prisonniers += individuelles->prisonniers;
}

// Fonction d'affichage des pertes
void afficher_somme_morts(Armee *armee) {
    Perte pertes_totales = {0, 0, 0, 0}; // Accumulateur global

    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            Perte pertes_regiment = {0, 0, 0, 0};  // Accumulateur par régiment

            for (int k = 0; k < N_COMPAGNIES; k++) {
                Perte pertes_compagnie = armee->divisions[i].regiments[j].compagnies[k].pertes;

#ifdef DEBUG
                time_t t = time(NULL);
                struct tm *current_time = localtime(&t);
                printf("À %02d:%02d Pertes de la compagnie C%d du régiment R%d de la division D%d\n",
                       current_time->tm_hour, current_time->tm_min, k, j, i);
                printf("\t Pertes : %d\n", pertes_compagnie.morts);
                printf("\t Blessés : %d\n", pertes_compagnie.blesses);
                printf("\t Ennemis tués : %d\n", pertes_compagnie.ennemis_morts);
                printf("\t Prisonniers : %d\n", pertes_compagnie.prisonniers);
#endif

                accumuler_pertes(&pertes_regiment, &pertes_compagnie); // Accumuler les pertes au niveau du régiment
            }

            accumuler_pertes(&pertes_totales, &pertes_regiment); // Accumuler les pertes au niveau global
        }
    }

    // Affichage des pertes totales
    printf("Total des pertes dans l'armée :\n");
    printf("Morts : %d, Blessés : %d, Ennemis tués : %d, Prisonniers : %d\n",
            pertes_totales.morts, pertes_totales.blesses, pertes_totales.ennemis_morts, pertes_totales.prisonniers);
            if (pertes_totales.morts > 35000 || pertes_totales.ennemis_morts > 35000) {
                    printf("           ___\n");
                    printf("         _/o o\\_\n");
                    printf("        /   -   \\\n");
                    printf("       |  \\___/  |\n");
                    printf("       \\_________/\n");
                    printf("        /       \\\n");
                    printf("       /|       |\\\n");
                    printf("      //|  WAR  |\\\\\n");
                    printf("     || |  IS   | ||\n");
                    printf("      \\\\|  BAD  |//\n");
                    printf("       ||_______||\n");
                    printf("       |         |\n");
                    printf("      /           \\\n");
                    printf("     |_____________|\n");
                    printf("         ||   ||\n");
                    printf("         ||   ||\n");
                    printf("         XX   XX\n");
                    printf("         oo   oo\n");
            }
}

// Fonction pour générer des pertes aléatoires pour chaque compagnie
void generer_pertes(Armee *armee) {
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                armee->divisions[i].regiments[j].compagnies[k].pertes.morts += rand() % 10;
                armee->divisions[i].regiments[j].compagnies[k].pertes.blesses += rand() % 10;
                armee->divisions[i].regiments[j].compagnies[k].pertes.ennemis_morts += rand() % 10;
                armee->divisions[i].regiments[j].compagnies[k].pertes.prisonniers += rand() % 10;
            }
        }
    }
}

// Fonction pour lancer une compagnie
void lancer_compagnie(int index) {
    pid_t pid = fork();
    if (pid == 0) {
        char index_str[10];
        snprintf(index_str, sizeof(index_str), "%d", index);
        execlp("./compagnie", "compagnie", index_str, NULL);
        perror("Erreur lors du lancement de la compagnie");
        exit(1);
    } else if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
}

// Fonction pour afficher l'état de la conquête
void afficher_etat_conquete(Armee *armee) {
    printf("=== État de la conquête ===\n");
    afficher_somme_morts(armee);  // Afficher les pertes totales de l'armée
    // Ajouter d'autres informations sur l'état de la conquête ici (par exemple, nombre de divisions actives, etc.)
}

int main() {
    // Allocation de la mémoire partagée pour l'armée
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

    // Initialiser les pertes de l'armée à zéro
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                armee->divisions[i].regiments[j].compagnies[k].pertes = (Perte){0, 0, 0, 0};
            }
        }
    }

    // Lancer les compagnies en parallèle
    for (int i = 0; i < N_DIVISIONS; i++) {
        for (int j = 0; j < N_REGIMENTS; j++) {
            for (int k = 0; k < N_COMPAGNIES; k++) {
                lancer_compagnie(i * N_REGIMENTS * N_COMPAGNIES + j * N_COMPAGNIES + k);
            }
        }
    }

    // Afficher l'état de la conquête toutes les 10 secondes
    while (1) {
        generer_pertes(armee);
        afficher_etat_conquete(armee);
        sleep(10);
    }

    // Attendre la fin de tous les processus enfants
    for (int i = 0; i < N_DIVISIONS * N_REGIMENTS * N_COMPAGNIES; i++) {
        wait(NULL);
    }

    // Détacher la mémoire partagée
    shmdt(armee);

    return 0;
}
