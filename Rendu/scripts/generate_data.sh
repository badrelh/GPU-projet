#!/bin/bash

# Fichier de sortie CSV
out="resultats.csv"
echo "version,taille,jeu_donnees,tile,threads,iterations,temps_ms" > "$out"

# Paramètres à explorer
tailles=(512 1024 2048 4096 8192)
donnees=("random" "ship" "moultdiehard130" "moultdiehard2474")
tuiles=(16 32 64 128)
threads=(1 2 4 8)
version="omp"

for taille in "${tailles[@]}"; do
  iter=$(( (100 * 8192 * 8192) / (taille * taille) ))

  for jeu in "${donnees[@]}"; do
    for tile in "${tuiles[@]}"; do
      for th in "${threads[@]}"; do

        echo " taille=$taille | jeu=$jeu | tile=$tile | threads=$th"

        export OMP_NUM_THREADS=$th
        export OMP_SCHEDULE=static

        cmd="./run -k life -v $version -a $jeu -s $taille -ts $tile -i $iter -n"
        echo "$cmd"
        output=$($cmd 2>&1)

        # Récupère la dernière ligne contenant uniquement un float
        temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$' | tail -1)

        # Ajout CSV
        echo "$version,$taille,$jeu,$tile,$th,$iter,$temps" >> "$out"
      done
    done
  done
done

echo "Fichier généré : $out"