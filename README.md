# Projet Programmation Parallèle IT224 2025 - Badr El Habbassi & Othmane Boutahri

## Présentation du Projet

Ce projet est basé sur la plateforme [EasyPAP](https://gforgeron.gitlab.io/easypap), qui fournit un environnement simple pour apprendre la programmation parallèle.

L'objectif est de paralléliser des calculs séquentiels sur des matrices 2D en utilisant des plateformes multicœur et GPU. Les différentes variantes (séquentielle, OpenMP, CUDA) sont implémentées et comparées.

La plateforme permet de :
- Choisir la taille des matrices ou le fichier image à charger
- Sélectionner le kernel à utiliser (e.g. `life`)
- Choisir la variante à utiliser (`seq`, `omp_task`, `omp_for`, etc.)
- Définir le nombre maximal d'itérations
- Activer le mode interactif ou le mode performance
- Monitorer les performances

Pour plus de détails, consultez le [Guide de démarrage](https://gforgeron.gitlab.io/easypap/doc/Getting_Started.pdf).

## Objectif du Projet

Analyser et comparer les performances de différentes implémentations du Jeu de la Vie de Conway :

- **Versions CPU** : Séquentielle, OpenMP (avec tiling, scheduling, first-touch, lazy evaluation)
- **Versions GPU** : CUDA (illustration du modèle GPU via une addition vectorielle)

L’objectif est de mesurer les gains de performances et d’évaluer la portabilité des optimisations sur différents jeux de données et architectures.



# Voir le dossier Rendu pour découvrir notre implémentation détaillée.