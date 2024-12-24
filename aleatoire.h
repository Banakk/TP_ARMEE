#ifndef ALEATOIRE_H
#define ALEATOIRE_H

// Initialiser le générateur de nombres aléatoires
void init_random();

// Générer un nombre aléatoire dans une plage donnée
int generer_nombre(int min, int max);

// Générer des pertes (morts, blessés, etc.)
int generer_perte();
int generer_prisonniers();
int generer_ennemis_morts();

#endif