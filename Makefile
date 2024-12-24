CC = gcc
CFLAGS = -Wall -g

# Fichiers objets
OBJ = general.o compagnie.o regiment.o division.o aleatoire.o sem_op.o shm_op.o

# Exécutables
EXEC = general compagnie regiment division

all: $(EXEC)

general: general.o sem_op.o aleatoire.o shm_op.o
	$(CC) -o general general.o sem_op.o aleatoire.o shm_op.o

compagnie: compagnie.o sem_op.o aleatoire.o shm_op.o
	$(CC) -o compagnie compagnie.o sem_op.o aleatoire.o shm_op.o

regiment: regiment.o sem_op.o aleatoire.o shm_op.o
	$(CC) -o regiment regiment.o sem_op.o aleatoire.o shm_op.o

division: division.o sem_op.o aleatoire.o shm_op.o
	$(CC) -o division division.o sem_op.o aleatoire.o shm_op.o

# Compilation des fichiers objets
aleatoire.o: aleatoire.c aleatoire.h
	$(CC) $(CFLAGS) -c aleatoire.c

compagnie.o: compagnie.c aleatoire.h shm_const.h sem_op.h
	$(CC) $(CFLAGS) -c compagnie.c

regiment.o: regiment.c aleatoire.h shm_const.h sem_op.h
	$(CC) $(CFLAGS) -c regiment.c

division.o: division.c aleatoire.h shm_const.h sem_op.h
	$(CC) $(CFLAGS) -c division.c

general.o: general.c aleatoire.h shm_const.h sem_op.h
	$(CC) $(CFLAGS) -c general.c

sem_op.o: sem_op.c shm_const.h
	$(CC) $(CFLAGS) -c sem_op.c

shm_op.o: shm_op.c shm_const.h sem_op.h
	$(CC) $(CFLAGS) -c shm_op.c

# Nettoyage
clean:
	rm -f *.o general compagnie regiment division
