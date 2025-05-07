#!/bin/bash

# ========================== CONFIGURATION ==========================

# Demander à l'utilisateur de saisir les paramètres
read -p "Nom du fichier CSV de sortie (ex: results/resultats_comp_versions_fixed.csv): " out
read -p "Tailles (séparées par un espace, ex: 512 1024 2048 4096): " -a tailles
read -p "Jeux de données (séparés par un espace, ex: random ship moultdiehard130): " -a donnees
read -p "Tailles de tuiles (séparées par un espace, ex: 16 32 64 128): " -a tuiles
read -p "Nombre de threads (séparés par un espace, ex: 1 2 4 8): " -a threads
read -p "Types de scheduling (séparés par un espace, ex: static dynamic guided): " -a schedules
read -p "Kernels (séparés par un espace, ex: life life_char life_ft): " -a kernel
read -p "Versions (séparées par un espace, ex: seq omp omp_lazy): " -a version


echo "nom fichier CSV de sortie: $out"
echo "tailles: ${tailles[@]}"
echo "jeux de données: ${donnees[@]}"
echo "tailles de tuiles: ${tuiles[@]}"
echo "nombre de threads: ${threads[@]}"
echo "types de scheduling: ${schedules[@]}"
echo "kernels: ${kernel[@]}"
echo "versions: ${version[@]}"

# Créer le fichier CSV et ajouter l'en-tête
echo "kernel,version,taille,jeu_donnees,tile,threads,schedule,iterations,temps_ms" > "$out"

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

              echo "taille=$taille | jeu=$jeu | tile=$tile | threads=$th | sched=$sched | version=$ver | kernel=$ker"

              # Lancer ./run avec parsing du temps affiché
              # output=$(./run -k $ker -v $ver -a $jeu -s $taille -ts $tile -i $iter -n 2>&1)
              echo "./run -k $ker -v $ver -a $jeu -s $taille -ts $tile -i $iter -n"
              # temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$' | tail -1)

              echo "$ker, $ver,$taille,$jeu,$tile,$th,$sched,$iter,$temps" >> "$out"
            done
          done
        done
      done
    done
  done
done

echo "Résultats générés dans $out"
