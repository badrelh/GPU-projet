#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Chargement
df = pd.read_csv("resultats.csv")

# Création du dossier figures si absent
os.makedirs("figures", exist_ok=True)

# Tri pour cohérence
df = df.sort_values(['taille', 'jeu_donnees', 'tile', 'threads'])

# ➤ Ajout colonne speedup vs threads=1
df_base = df[df['threads'] == 1].copy()
df_base = df_base.rename(columns={"temps_ms": "temps_seq"})
df_merged = df.merge(df_base[['taille', 'jeu_donnees', 'tile', 'temps_seq']],
                     on=['taille', 'jeu_donnees', 'tile'])
df_merged['speedup'] = df_merged['temps_seq'] / df_merged['temps_ms']

# ➤ Pour chaque taille et chaque jeu : heatmap (tile × threads)
for taille in sorted(df_merged['taille'].unique()):
    for jeu in sorted(df_merged['jeu_donnees'].unique()):
        data = df_merged[(df_merged['taille'] == taille) & (df_merged['jeu_donnees'] == jeu)]
        pivot = data.pivot(index='tile', columns='threads', values='speedup')
        if pivot.empty:
            continue
        plt.figure(figsize=(6, 4))
        sns.heatmap(pivot, annot=True, fmt=".2f", cmap="crest")
        plt.title(f"Speedup (taille={taille}, jeu={jeu})")
        plt.xlabel("Threads")
        plt.ylabel("Tile size")
        plt.tight_layout()
        filename = f"figures/speedup_t{taille}_{jeu}.pdf"
        plt.savefig(filename)
        print(f" Figure sauvegardée : {filename}")

# ➤ Graphe 2D : speedup vs threads pour une tile et taille donnée
taille_focus = 2048
tile_focus = 64
jeu_focus = "random"
subset = df_merged[(df_merged['taille'] == taille_focus) &
                   (df_merged['tile'] == tile_focus) &
                   (df_merged['jeu_donnees'] == jeu_focus)]
plt.figure(figsize=(6,4))
plt.plot(subset['threads'], subset['speedup'], marker='o', linestyle='-')
plt.xlabel("Nombre de threads")
plt.ylabel("Speedup")
plt.title(f"Speedup ({jeu_focus}, taille={taille_focus}, tile={tile_focus})")
plt.grid()
plt.tight_layout()
plt.savefig(f"figures/curve_speedup_t{taille_focus}_{jeu_focus}_tile{tile_focus}.pdf")
print(" Graphe 2D sauvegardé.")