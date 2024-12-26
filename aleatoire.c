#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

void init_random() {
    srand(time(NULL) ^ getpid());
}

int generer_nombre(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int generer_perte() {
    return generer_nombre(0, 150);
}

int generer_prisonniers() {
    return generer_nombre(0, 150);
}

int generer_ennemis_morts() {
    return generer_nombre(0, 150); 
}