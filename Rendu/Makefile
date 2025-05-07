all: run

run:
	sh scripts/bash/generate_general_result.sh

empreinte_memoire:
	sh scripts/bash/generate_result_comp_emp_memoire.sh

comparaison_versions:
	sh scripts/bash/generate_general_result.sh << EOF
	results/comparaison_versions.csv
	1024
	random ship moultdiehard130
	32
	1 2 4 8 12 24 48
	static
	life life_char life_ft
	seq omp omp_lazy
	eof

comparaison_omp_scheduling:
	sh scripts/bash/generate_result_parametrée.sh << EOF
	results/comparaison_omp_scheduling.csv
	1024
	random
	32
	1 2 4 8 12 24 48
	static dynamic guided
	life_ft
	omp
	eof

comparaison_taille_tiling:
	sh scripts/bash/generate_result_parametrée.sh << EOF
	results/comparaison_taille_tiling.csv
	1024
	random
	16 32 64 128
	1 2 4 8 12 24 48
	static
	life_ft
	omp
	eof

comparaison_jeux_donnees:
	sh scripts/bash/generate_result_parametrée.sh << EOF
	results/comparaison_jeux_donnees.csv
	1024
	random ship moultdiehard130 moultdiehard2474
	32
	1 2 4 8 12 24 48
	static
	life_ft
	omp
	eof

# Graphiques
graph_empreinte_memoire:
	python3 scripts/python/generate_result_plots.py

graph_versions: comparaison_versions
	python3 scripts/python/graph_comparaison_versions.py

graph_omp_scheduling: comparaison_omp_scheduling
	python3 scripts/python/graph_part1_omp.py

graph_tiling:
	python3 scripts/python/graph_part2_omp_lazy.py

graph_jeux_donnees:
	python3 scripts/python/graph_q2_generate.py

results_dir:
	mkdir -p results

figures_dir:
	mkdir -p figures

%.csv: results_dir
%.png: figures_dir


all_csv: empreinte_memoire comparaison_versions comparaison_omp_scheduling comparaison_taille_tiling comparaison_jeux_donnees

all_graphs: graph_empreinte_memoire graph_versions graph_omp_scheduling graph_tiling graph_jeux_donnees


help:
	@echo "Usage : make [cible]"
	@echo "Cibles disponibles :"
	@echo "  all                     - Exécute le script général"
	@echo "  empreinte_memoire       - Génère le fichier CSV de comparaison d'empreinte mémoire"
	@echo "  comparaison_versions    - Génère le CSV des versions seq, omp, omp_lazy"
	@echo "  comparaison_omp_scheduling - Analyse le scheduling OpenMP"
	@echo "  comparaison_taille_tiling - Analyse la taille des tuiles"
	@echo "  comparaison_jeux_donnees - Compare les jeux de données"
	@echo "  all_csv                 - Génère tous les fichiers CSV"
	@echo "  all_graphs              - Génère tous les graphiques"
	@echo "  clean                   - Supprime tous les CSV et PNG"



clean:
	rm -f results/*.csv figures/*.png
	echo "Tous les fichiers CSV et PNG ont été supprimés."

.PHONY: all run empreinte_memoire comparaison_versions comparaison_omp_scheduling comparaison_taille_tiling comparaison_jeux_donnees graph_empreinte_memoire graph_versions graph_omp_scheduling graph_tiling graph_jeux_donnees clean
