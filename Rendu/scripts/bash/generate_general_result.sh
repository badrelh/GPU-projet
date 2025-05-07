#!/bin/bash

# ========================== CONFIGURATION ==========================
out="results/resultats_comp_versions_fixed.csv"  # nom du fichier CSV de sortie
echo "kernel,version,taille,jeu_donnees,tile,threads,schedule,iterations,temps_ms" > "$out"

# Active les dimensions que tu veux explorer ici :
tailles=(1024)                    # ou (512 1024 2048 4096)
donnees=("random" "ship" "moultdiehard130" )                # ou ("random" "ship" "moultdiehard130")
tuiles=(32)                       # ou (16 32 64 128)
threads=(1 2 4 8 12 24 48)        # ou (1 2 4 8)
schedules=("static")             # ou ("static" "dynamic" "guided")

kernel=("life" "life_char" "life_ft" )
version=("seq" "omp" "omp_lazy" )
# ===================================================================



for taille in "${tailles[@]}"; do
  iter=$(( (100 * 8192 * 8192) / (taille * taille) ))
  for jeu in "${donnees[@]}"; do
    for tile in "${tuiles[@]}"; do
      for th in "${threads[@]}"; do
        for ker in "${kernel[@]}"; do
          export OMP_NUM_THREADS=$th
          for sched in "${schedules[@]}"; do
            for ver in "${version[@]}"; do

              export OMP_SCHEDULE=$sched

              echo "taille=$taille | jeu=$jeu | tile=$tile | threads=$th | sched=$sched | version=$version | kernel=$kernel"

              # Lancer ./run avec parsing du temps affiché
              output=$(./run -k $ker -v $ver -a $jeu -s $taille -ts $tile -i $iter -n 2>&1)
              echo "./run -k $ker -v $ver -a $jeu -s $taille -ts $tile -i $iter -n"
              temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$' | tail -1)

              echo "$ker, $ver,$taille,$jeu,$tile,$th,$sched,$iter,$temps" >> "$out"
              
            done
          done
        done
      done
    done
  done
done

echo "Résultats générés dans $out"