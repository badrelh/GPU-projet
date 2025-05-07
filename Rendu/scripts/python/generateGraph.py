import pandas as pd
import matplotlib.pyplot as plt

# Chargement des données
df = pd.read_csv("resultats.csv")

# Optionnel : trier les tailles si besoin
df = df.sort_values(by="taille")

# Génération du graphe
plt.figure(figsize=(10, 6))

# Pour chaque stratégie (version), tracer les courbes
for version in df["version"].unique():
    sous_df = df[df["version"] == version]
    plt.plot(sous_df["taille"], sous_df["temps_ms"], label=version, marker='o')

plt.title("Temps d'exécution en fonction de la taille (travail constant)")
plt.xlabel("Taille du jeu de données (s)")
plt.ylabel("Temps d'exécution (ms)")
plt.legend()
plt.grid(True)
plt.tight_layout()

# Sauvegarde
plt.savefig("performance_vs_taille.png", dpi=300)
plt.show()
