#!/bin/bash

# Fichier CSV de sortie
echo "version,taille,iterations,temps_ms" > resultats.csv

# Tableaux de tailles et versions
tailles=(512 1024 2048 4096 8180)
versions=("seq" "omp")

for taille in "${tailles[@]}"; do
  # Calcul des itérations pour travail constant
  iter=$(( (100 * 8192 * 8192) / (taille * taille) ))

  for version in "${versions[@]}"; do
    echo "▶️  version=$version | taille=$taille | iterations=$iter"

    # Sélection de la variante à passer à ./run
    case $version in
      seq)
        VARIANT="seq"
        export OMP_NUM_THREADS=1
        ;;
      omp_collapse)
        VARIANT="omp"
        export OMP_NUM_THREADS=8
        export OMP_SCHEDULE=static
        ;;
      omp_dynamic)
        VARIANT="omp"
        export OMP_NUM_THREADS=8
        export OMP_SCHEDULE=dynamic
        ;;
    esac

    # Exécuter et capturer la sortie
    echo " ./run -k life -a random -s $taille -v $VARIANT -i $iter -n"
    output=$(./run -k life -a random -s $taille -v $VARIANT -i $iter -n  2>&1)

    # Extraire le temps en ms depuis la dernière ligne (ex: "1005.778")
    temps=$(echo "$output" | grep -Eo '^[0-9]+\.[0-9]+$')
    echo "$temps"

    # Ajout dans le CSV
    echo "$version,$taille,$iter,$temps" >> resultats.csv
  done
done