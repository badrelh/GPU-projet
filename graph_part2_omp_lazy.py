#!/usr/bin/env python3

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Chargement depuis le bon fichier
df = pd.read_csv("results/resultats_lazy.csv")

# Filtrage : taille=1024, input=random
df = df[(df['taille'] == 1024) & (df['jeu_donnees'] == 'random')]

# Nettoyage
df['temps_ms'] = pd.to_numeric(df['temps_ms'], errors='coerce')
df = df.dropna()

# Calcul du speedup vs 1 thread (réf locale par combo fixe)
ref = df[df['threads'] == 1].rename(columns={"temps_ms": "ref"})
df = df.merge(ref[['tile', 'schedule', 'ref']], on=['tile', 'schedule'])
df['speedup'] = df['ref'] / df['temps_ms']

# Dossier de sortie
outdir = "figures/lazy"
os.makedirs(outdir, exist_ok=True)

sns.set_theme(style="whitegrid", font_scale=1.2)

# ===== Courbe 1 : Variation du scheduling (tile fixée) =====
tile_fixed = 32
df_sched = df[df['tile'] == tile_fixed]

plt.figure(figsize=(8, 5))
for sched in sorted(df_sched['schedule'].unique()):
    s = df_sched[df_sched['schedule'] == sched]
    plt.plot(s['threads'], s['speedup'], marker='o', label=sched)

plt.title(f"Speedup – Tiling fixé à {tile_fixed} (grille=1024, input=random)")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend(title="Scheduling")
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig(f"{outdir}/lazy_sched_variation_tile{tile_fixed}.png", dpi=300)
print(f"✅ Figure : lazy_sched_variation_tile{tile_fixed}.png")

# ===== Courbe 2 : Variation de la taille de tuile (schedule fixé) =====
sched_fixed = "static"
df_tile = df[df['schedule'] == sched_fixed]

plt.figure(figsize=(8, 5))
for tile in sorted(df_tile['tile'].unique()):
    t = df_tile[df_tile['tile'] == tile]
    plt.plot(t['threads'], t['speedup'], marker='o', label=f"tile {tile}")

plt.title(f"Speedup – Scheduling fixé à {sched_fixed} (grille=1024, input=random)")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend(title="Tiling")
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig(f"{outdir}/lazy_tiling_variation_sched-{sched_fixed}.png", dpi=300)
print(f"✅ Figure : lazy_tiling_variation_sched-{sched_fixed}.png")
