import pandas as pd
import matplotlib.pyplot as plt

file_path = 'results/resultats_comp_versions_fixed.csv'
data = pd.read_csv(file_path, sep=',')

data.columns = data.columns.str.strip()
for col in ['kernel', 'version', 'jeu_donnees', 'schedule']:
    if col in data.columns and data[col].dtype == 'object':
        data[col] = data[col].str.strip()

target_kernels = ['life', 'life_char', 'life_ft']
target_versions = ['seq', 'omp', 'omp_lazy']

data_filtered = data[
    data['kernel'].isin(target_kernels) &
    data['version'].isin(target_versions)
].copy()

data_filtered['temps_ms'] = pd.to_numeric(data_filtered['temps_ms'], errors='coerce')
data_filtered['threads'] = pd.to_numeric(data_filtered['threads'], errors='coerce')

data_filtered.dropna(subset=['temps_ms', 'threads'], inplace=True)

summary = data_filtered.groupby(['kernel', 'jeu_donnees', 'version', 'threads'])['temps_ms'].mean().reset_index()

if not data_filtered.empty:
    taille_grille = data_filtered['taille'].iloc[0]
    taille_tuile = data_filtered['tile'].iloc[0]
    schedule_type = data_filtered['schedule'].iloc[0]
else:
    taille_grille, taille_tuile, schedule_type = 'N/A', 'N/A', 'N/A'

jeux_de_donnees_uniques = summary['jeu_donnees'].unique()

version_colors = {'seq': 'green', 'omp': 'blue', 'omp_lazy': 'orange'}
kernel_styles = {
    'life': {'linestyle': '-', 'marker': 'o'},
    'life_char': {'linestyle': '--', 'marker': 's'},
    'life_ft': {'linestyle': ':', 'marker': '^'}
}


for jeu_donnees in jeux_de_donnees_uniques:
    fig, ax = plt.subplots(figsize=(12, 8)) 

    data_subplot = summary[summary['jeu_donnees'] == jeu_donnees]
    
    for kernel_name in target_kernels:
        data_kernel = data_subplot[data_subplot['kernel'] == kernel_name]
        for version_name in target_versions:
            data_version = data_kernel[data_kernel['version'] == version_name]
            
            if not data_version.empty:
                style = kernel_styles.get(kernel_name, {'linestyle': '-', 'marker': '.'})
                ax.plot(data_version['threads'], data_version['temps_ms'],
                        label=f'{kernel_name} - {version_name}',
                        color=version_colors[version_name],
                        linestyle=style['linestyle'],
                        marker=style['marker'],
                        markersize=5,  
                        linewidth=1.2) 

    ax.set_xlabel('Nombre de threads', fontsize=12)
    ax.set_ylabel("Temps d'exécution moyen (ms) - Échelle Logarithmique", fontsize=12) 
    
    ax.set_yscale('log')
    
    ax.legend(fontsize='medium', title='Kernel - Version') 
    ax.grid(True, which="both", ls="--", linewidth=0.5, alpha=0.7) 
    
    titre_figure = (f'Performances pour le jeu de données: {jeu_donnees}\n'
                    f'(Grille: {taille_grille}, Tuile: {taille_tuile}, Schedule: {schedule_type})')
    ax.set_title(titre_figure, fontsize=14) 
    
    plt.tight_layout()

plt.show()
