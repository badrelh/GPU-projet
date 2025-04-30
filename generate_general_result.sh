#!/bin/bash

# ========================== CONFIGURATION ==========================
out="results/resultats_q1.csv"  # nom du fichier CSV de sortie
echo "version,taille,jeu_donnees,tile,threads,schedule,iterations,temps_ms" > "$out"

# Active les dimensions que tu veux explorer ici :
tailles=(1024)                    # ou (512 1024 2048 4096)
donnees=("random")                # ou ("random" "ship" "moultdiehard130")
tuiles=(16 32)                       # ou (16 32 64 128)
threads=(1 2 4 8 12 24 48)        # ou (1 2 4 8)
schedules=("static" "dynamic" "guided")             # ou ("static" "dynamic" "guided")

version="omp"
# ===================================================================



for taille in "${tailles[@]}"; do
  iter=$(( (100 * 8192 * 8192) / (taille * taille) ))
  for jeu in "${donnees[@]}"; do
    for tile in "${tuiles[@]}"; do
      for th in "${threads[@]}"; do
        export OMP_NUM_THREADS=$th
        for sched in "${schedules[@]}"; do
          export OMP_SCHEDULE=$sched

          echo "taille=$taille | jeu=$jeu | tile=$tile | threads=$th | sched=$sched"

          # Lancer ./run avec parsing du temps affiché
          output=$(./run -k life -v $version -a $jeu -s $taille -ts $tile -i $iter -n 2>&1)
          echo "./run -k life -v $version -a $jeu -s $taille -ts $tile -i $iter -n"
          temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$' | tail -1)

          echo "$version,$taille,$jeu,$tile,$th,$sched,$iter,$temps" >> "$out"
        done
      done
    done
  done
done

echo "Résultats générés dans $out"