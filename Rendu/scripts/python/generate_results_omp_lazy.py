import pandas as pd
import matplotlib.pyplot as plt

file_path = 'results/resultats_omp_lazy_2.csv' 
data = pd.read_csv(file_path, sep=',') 

data.columns = data.columns.str.strip()
for col in ['kernel', 'version', 'jeu_donnees', 'schedule']: 
    if col in data.columns and data[col].dtype == 'object':
        data[col] = data[col].str.strip()

data_life = data[(data['kernel'] == 'life') & (data['version'].isin(['omp', 'omp_lazy']))].copy() 

data_life['temps_ms'] = pd.to_numeric(data_life['temps_ms'], errors='coerce')
data_life['threads'] = pd.to_numeric(data_life['threads'], errors='coerce')

summary = data_life.groupby(['jeu_donnees', 'version', 'threads'])['temps_ms'].mean().reset_index()

taille_grille = 4096
taille_tuile = 32
schedule_type = 'static'

jeux_de_donnees_uniques = summary['jeu_donnees'].unique()

fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(15, 10), sharex=True, sharey=False) 
axes_flat = axes.flatten() 

colors = {'omp': 'blue', 'omp_lazy': 'orange'}
markers = {'omp': 'o', 'omp_lazy': 'x'}

for i, jeu_donnees in enumerate(jeux_de_donnees_uniques):
    if i < 4: 
        ax = axes_flat[i]
        
        data_subset_jeu = summary[summary['jeu_donnees'] == jeu_donnees]
        
        for version in ['omp', 'omp_lazy']:
            data_to_plot = data_subset_jeu[data_subset_jeu['version'] == version]
            if not data_to_plot.empty:
                ax.plot(data_to_plot['threads'], data_to_plot['temps_ms'], 
                        label=version, color=colors[version], marker=markers[version])
        
        ax.set_title(f'Jeu de données: {jeu_donnees}')
        ax.set_xlabel('Nombre de threads')
        ax.set_ylabel("Temps d'exécution moyen (ms)")
        ax.legend()
        ax.grid(True)
    else:
        break
    
for j in range(i + 1, len(axes_flat)):
    fig.delaxes(axes_flat[j])

titre_figure = (f'Comparaison omp vs omp_lazy (Kernel: life)\n'
                f'Taille grille: {taille_grille}, Taille tuile: {taille_tuile}, Schedule: {schedule_type}')
fig.suptitle(titre_figure, fontsize=16)

plt.tight_layout(rect=[0, 0, 1, 0.93])
plt.show()
