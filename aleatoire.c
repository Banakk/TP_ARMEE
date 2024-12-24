#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Pour getpid()

// Initialiser le générateur de nombres aléatoires
void init_random() {
    srand(time(NULL) ^ getpid()); // Combinaison de temps et PID pour plus d'unicité
}

// Générer un nombre aléatoire dans une plage donnée
int generer_nombre(int min, int max) {
    return rand() % (max - min + 1) + min; // Génère un nombre entre min et max inclus
}

// Générer des pertes aléatoires
int generer_perte() {
    return generer_nombre(0, 150); // Entre 0 et 30 inclus
}

// Générer un nombre de prisonniers aléatoire
int generer_prisonniers() {
    return generer_nombre(0, 150); // Entre 0 et 40 inclus
}

// Générer un nombre d'ennemis morts aléatoire
int generer_ennemis_morts() {
    return generer_nombre(0, 150); // Entre 0 et 110 inclus
}