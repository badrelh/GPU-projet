import pandas as pd
import matplotlib.pyplot as plt

# Chargement des données
df = pd.read_csv("resultats.csv")
df["temps_ms"] = pd.to_numeric(df["temps_ms"], errors='coerce')
df = df.dropna()

# Définition des couleurs par version
couleurs = {
    "seq": "red",
    "omp": "blue",
    "omp_collapse": "green",
    "omp_dynamic": "orange",
    "omp_lazy": "purple"
}

# Création du graphique
plt.figure(figsize=(10, 6))

# Tracer les lignes avec couleurs personnalisées
for version in df["version"].unique():
    sous_df = df[df["version"] == version]
    couleur = couleurs.get(version, None)  # défaut : matplotlib choisit
    plt.plot(sous_df["taille"], sous_df["temps_ms"],
             marker="o", label=version, color=couleur)

plt.title("Temps d'exécution (ms) en fonction de la taille de la grille")
plt.xlabel("Taille du problème (DIM)")
plt.ylabel("Temps d'exécution (ms)")
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.savefig("temps_execution_couleur.png", dpi=300)
plt.show()