
# Projet programmation parallèle IT224 2025 - Badr el habbassi & Othmane boutahri 

# Objectif du Projet

Ce projet vise à comparer différentes implémentations du célèbre Jeu de la Vie de Conway (Game of Life), en évaluant les performances de versions CPU (OpenMP, variantes d’allocation mémoire) et GPU (CUDA).



# Arborescence de Rendu 
Rendu/
├── Annex/                      # Informations système et figures supplémentaires
├── C/                          # Versions CPU du Game of Life  
├── Cuda/                       # Versions GPU avec CUDA
├── data/results/              # Résultats expérimentaux (CSV)
├── figures/                    # Graphiques générés à partir des résultats
├── scripts/               # Scripts pour générer les résultats et les graphiques
├── README.md                   # Ce fichier
├── rpt/                        # (Rapport du projet, si présent)



# Commandes a éxécuter 


## Génération de fichier csv 
Utiliser la commande :
- make empreinte mémoire : Génère le fichier empreinte_memoire.csv contenant la comparaison de la mémoire utilisée (footprint_bytes) entre les versions life et life_char.
La comparaison est faite avec un nombre de threads fixé à 1 (exécution séquentielle).
- make comparaison versions :Génère le fichier comparaison_versions.csv, contenant les performances des différentes versions du programme (life, life_char, life_ft) avec plusieurs configurations :

    Version séquentielle (seq)

    Version parallèle OpenMP (omp)

    Version OpenMP avec allocation "lazy" (omp_lazy)

Les tests sont menés avec une grille de 1024x1024, un tiling de 32, et un nombre de threads variant de 1 à 48, sur différents jeux de données (random, ship, moultdiehard130...).

- make comparaison omp_scheduling : Génère le fichier comparaison_omp_scheduling.csv, qui mesure l’impact des différents types de scheduling OpenMP (static, dynamic, guided) sur les performances selon le nombre de threads et la taille de tuile.

- make comparaison taille de tiling  : Génère le fichier comparaison_taille_tiling.csv, comparant les performances selon différentes tailles de tuiles (16, 32, 64, 128) pour un même jeu de données et version.

- make comparaison de jeux de données : Génère le fichier comparaison_jeux_donnees.csv, comparant les performances de la version OpenMP sur différents jeux de données (random, ship, moultdiehard130, moultdiehard2474), avec un tiling fixe de 32 et schedule=static.



## Génération de graphes 
Pour visualiser les résultats de performance et de consommation mémoire, plusieurs scripts Python sont disponibles pour générer automatiquement des graphes à partir des fichiers CSV produits précédemment.

Assurez-vous d’avoir les bibliothèques suivantes installées : matplotlib pandas seaborn .

### Génération automatique via Make

Les graphes peuvent être générés en utilisant les commandes make graph_* suivantes :

    make graph_empreinte_memoire
    → Compare visuellement l’empreinte mémoire (footprint_bytes) des versions life et life_char (threads = 1).

    make graph_versions
    → Compare les temps d’exécution des versions seq, omp, et omp_lazy pour différents jeux de données et nombres de threads.

    make graph_omp_scheduling
    → Affiche les performances selon la stratégie de planification OpenMP (static, dynamic, guided) en fonction du nombre de threads et de la taille des tuiles.

    make graph_tiling
    → Montre l’impact des tailles de tuiles (16, 32, 64, 128) sur les performances selon le nombre de threads.

    make graph_jeux_donnees
    → Compare les performances sur différents jeux de données avec une configuration fixe.

### Exemple : 

make graph_versions

Chaque commande produit un ou plusieurs fichiers .png dans le répertoire plots/results/, avec un nom explicite selon les paramètres.





## Vérification de correction du code - Comparaison des hashage 

Afin de valider que vos optimisations (parallélisation, lazy allocation, etc.) ne modifient pas le comportement fonctionnel du programme, une vérification sémantique peut être effectuée à l’aide de l’option -sh.
Principe

L’option -sh permet de générer un hash de l’état final de la grille (l’image résultante après toutes les itérations). Deux programmes peuvent être considérés fonctionnellement équivalents si, pour un même jeu de données, ils produisent le même hash.

Cela permet de s'assurer que les optimisations respectent bien la logique du programme séquentiel original (life_seq).


Nous avons fait un script bash pour faciliter l'utilisation 

## Partie Cuda et GPU 

Cette section explore une implémentation simple d’un kernel CUDA pour réaliser l’addition de deux vecteurs, comparée à une version CPU de référence. L’objectif est double :

    - illustrer le modèle de programmation GPU avec CUDA,

    - comparer les performances CPU vs GPU sur des tailles de vecteurs croissantes.


### Fichiers

    kernel.cu : exécute une addition vectorielle unique sur le GPU, avec vérification du résultat et mesure du temps.

    kernel_for_plot.cu : répète l’opération pour différentes tailles N, enregistre les temps d'exécution CPU et GPU dans timing_results.csv.

    plotCuda.py : lit le fichier CSV et trace un graphe comparatif des performances CPU/GPU.


### Compilation

Utilise nvcc (le compilateur NVIDIA CUDA) pour compiler les programmes :

make        # Compile kernel
make plot   # Compile kernel_for_plot.cu et génère le graphe


### Visualisation

Le script Python plotCuda.py trace l’évolution des temps CPU et GPU pour différentes tailles de vecteur (de 2¹⁰ à 2²⁵), avec échelles logarithmiques pour mieux observer les variations :

    x-axis : taille du vecteur (N)

    y-axis : temps d’exécution en secondes

    courbes : comparaison directe des performances CPU vs GPU


### Exemple de sortie console

CPU addition verified successfully!

CPU Time (): 1 seconds
GPU Kernel Time: 0.002134 seconds
Data Transfer Time (H->D): 0.008473 seconds
Data Transfer Time (D->H): 0.004762 seconds