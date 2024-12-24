#ifndef SHM_CONST_H
#define SHM_CONST_H

// Définition des constantes
#define N_DIVISIONS 3
#define N_REGIMENTS 3
#define N_COMPAGNIES 15
#define SHM_SIZE         sizeof(int)
#define CLE_MEMOIRE  0x555    // La clé de la mémoire partagée
#define CLE_SEM           0x111      // Clé pour le sémaphore

// Structures représentant la hiérarchie militaire

typedef struct {
    int morts;
    int blesses;
    int ennemis_morts;
    int prisonniers;
} Perte;

typedef struct {
    Perte pertes;
} Compagnie;

typedef struct {
    Compagnie compagnies[N_COMPAGNIES];
} Regiment;

typedef struct {
    Regiment regiments[N_REGIMENTS];
} Division;

typedef struct {
    Division divisions[N_DIVISIONS];
} Armee;

#endif
