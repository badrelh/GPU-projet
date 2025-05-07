#!/bin/bash

# ========================== CONFIGURATION ==========================
out="results/empreinte_memoire.csv"  # nom du fichier CSV de sortie
echo "kernel,version,taille,jeu_donnees,tile,threads,footprint_bytes,temps_ms" > "$out"

# Paramètres explorés
tailles=(512 1024 2048 4096)
donnees=("random")
tuiles=(32)
threads=(1)  # On fixe à 1 pour isoler l'effet mémoire
kernel=("life" "life_char")
version=("seq" "omp")
# ===================================================================

for taille in "${tailles[@]}"; do
  iter=100
  for jeu in "${donnees[@]}"; do
    for tile in "${tuiles[@]}"; do
      for th in "${threads[@]}"; do
        export OMP_NUM_THREADS=$th
        for ker in "${kernel[@]}"; do
          for ver in "${version[@]}"; do
            echo "taille=$taille | jeu=$jeu | tile=$tile | kernel=$ker | version=$ver"

            # Exécution avec debug mémoire
            output=$(../run -k $ker -v $ver -a $jeu -s $taille -ts $tile -i $iter -n --debug u 2>&1)

            # Extraction de la mémoire et du temps
            mem=$(echo "$output" | grep -i "Memory footprint" | grep -Eo '[0-9]+' | tail -1)
            mem=$((mem * 2))
            temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$' | tail -1)

            echo "$ker,$ver,$taille,$jeu,$tile,$th,$mem,$temps" >> "$out"
          done
        done
      done
    done
  done
done

echo "Résultats générés dans $out"