# TP_ARMEE

# Objectif
On désire faire un jeu de simulation de conquête militaire en C.

Un général dispose de troupes organisées en divisions. Chaque division est organisée en régiments. Chaque régiment est divisé en compagnies. Chaque compagnie dispose d’hommes commandés par un capitaine. L’idée est lorsqu’une conquête est lancée, des divisions sont lancées dans les batailles, donc des régiments, donc des compagnies aussi. Il en résulte des pertes humaines (morts et des blessés) mais aussi des pertes coté ennemi (morts et prisonniers). Les informations sur les pertes sont remontées à partir des compagnies aux niveaux supérieurs et ainsi de suite jusqu’au général.


Le général va lancer dans la conquête : • Une armée (commandée par le général) est composée de 3 divisions. • Chaque division est composée de 3 régiments. • Chaque régiment est composé de 5 compagnies, chaque compagnie de 150 hommes commandés par un capitaine



# Exemple :

À 12:17 : Compagnie B du régiment R1 de la division D2 : envoie : morts 5, blessés 2, ennemis morts 10, prisonniers : 5 Lorsque le régiment R1 reçoit cette information, il la fait parvenir à la division D2 qui ellemême la fait parvenir à l’armée.

# Implémentation

On créera autant de processus que de structures militaires définies dans l’armée. Au lancement de la simulation, on crée : • l’armée • les divisions • les régiments • les compagnies Les échanges entre les différents niveaux se feront par des segments de mémoire partagée Le processus de plus haut niveau affichera l’état de la conquête toutes les 10s


# Évolution

Évolutions possibles On pourrait imaginer que les différentes structures remontent des informations sur la progression sur le terrain (avancée de x km, recul de y km...)


# Lancer le jeu 


```bash
chmod +x build.sh && ./build.sh
```

Comme le jeu fonctionne avec des IPC (Interprocess Communications) il faut donc nettoyer la mémoire au cas où il resterait des segments alloués au jeu. Pour nettoyer :

```bash
chmod +x clean.sh && ./clean.sh
```


