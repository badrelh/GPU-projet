import pandas as pd
import matplotlib.pyplot as plt
import os

# Chargement des fichiers CSV
lazy_df = pd.read_csv('results/resultats_lazy.csv')
ft_omp_df = pd.read_csv('results/resultats_ft_omp.csv')
omp_df = pd.read_csv('results/resultats_omp.csv')
empreinte_df = pd.read_csv('results/resultats_empreinte_memoire.csv')

# Filtrage des données avec schedule 'static'
lazy_static = lazy_df[lazy_df['schedule'] == 'static']
ft_omp_static = ft_omp_df[ft_omp_df['schedule'] == 'static']
omp_static = omp_df[omp_df['schedule'] == 'static']
empreinte_static = empreinte_df[empreinte_df['schedule'] == 'static']

# Préparation du graphique
plt.figure(figsize=(10, 6))

# Tracer les courbes
plt.plot(lazy_static['threads'], lazy_static['temps_ms'], marker='o', label='Version Lazy')
plt.plot(ft_omp_static['threads'], ft_omp_static['temps_ms'], marker='o', label='Version First Touch (OMP)')
plt.plot(omp_static['threads'], omp_static['temps_ms'], marker='o', label='Version améliorée Tiled (OMP)')
plt.plot(empreinte_static['threads'], empreinte_static['temps_ms'], marker='o', label='Version Empreinte Mémoire Réduite')

# Configuration du graphique
plt.title('Comparaison des temps d\'exécution selon le nombre de threads (schedule static)')
plt.xlabel('Nombre de threads')
plt.ylabel('Temps (ms)')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()

# Création du dossier de sortie
output_dir = 'figures/comparaison'
os.makedirs(output_dir, exist_ok=True)

# Sauvegarde du graphique
output_path = os.path.join(output_dir, 'comparaison_static_schedule.png')
plt.savefig(output_path, dpi=300)
plt.show()
