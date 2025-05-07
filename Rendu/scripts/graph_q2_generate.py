#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

df = pd.read_csv("results/resultats_q2.csv")
df['temps_ms'] = pd.to_numeric(df['temps_ms'], errors='coerce')
df = df.dropna()

# Calcul du speedup par rapport à 1 thread pour chaque configuration
ref = df[df['threads'] == 1].rename(columns={"temps_ms": "ref"})
df = df.merge(ref[['taille', 'jeu_donnees', 'tile', 'schedule', 'ref']], on=['taille', 'jeu_donnees', 'tile', 'schedule'])
df['speedup'] = df['ref'] / df['temps_ms']


# Dossier de sortie pour les figures
output_dir = "figures/q2"
os.makedirs(output_dir, exist_ok=True)

# Style des graphiques
sns.set_theme(style="whitegrid", font_scale=1.2)

# Graphiques : speedup vs threads pour chaque (taille, tile, schedule), regroupé par jeu de données
for taille in sorted(df['taille'].unique()):
    for tile in sorted(df['tile'].unique()):
        for sched in sorted(df['schedule'].unique()):
            subset = df[(df['taille'] == taille) &
                        (df['tile'] == tile) &
                        (df['schedule'] == sched)]
                
            if subset.empty:
                continue

            plt.figure(figsize=(8, 5))
            
            # Regrouper par jeu de données et tracer
            for jeu in sorted(subset['jeu_donnees'].unique()):
                s = subset[subset['jeu_donnees'] == jeu]
                plt.plot(s['threads'], s['speedup'], marker='o', label=jeu)

            plt.title(f"Speedup vs Threads – Taille={taille}, Tiling={tile}, Scheduling={sched}")
            plt.xlabel("Threads")
            plt.ylabel("Speedup")
            plt.xticks(subset['threads'].unique())
            plt.legend(title="Jeu de données")
            plt.grid(True, linestyle="--", alpha=0.6)
            plt.tight_layout()
            
            filename = f"{output_dir}/speedup_{taille}_{tile}_{sched}.png"
            plt.savefig(filename, dpi=300)
            print(f"✅ Figure générée : {filename}")