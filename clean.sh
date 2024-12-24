#!/bin/sh

# Récupérer les IDs des segments de mémoire partagée pour le groupe "staff"
shm_ids=$(ipcs -m | grep "staff" | awk '{print $2}')

# Supprimer les segments de mémoire partagée
for id in $shm_ids
do
  echo "Suppression du segment de mémoire partagée ID: $id"
  ipcrm -m $id
done

# Récupérer les IDs des sémaphores pour le groupe "staff" (si applicable)
sem_ids=$(ipcs -s | grep "staff" | awk '{print $2}')

# Supprimer les sémaphores
for id in $sem_ids
do
  echo "Suppression du sémaphore ID: $id"
  ipcrm -s $id
done
