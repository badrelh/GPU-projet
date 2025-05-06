import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Lire les résultats depuis le fichier CSV
data = pd.read_csv("timing_results.csv")

# Extraire les données
N_values = data["N"]
cpu_time = data["CPU_Time"]  # Temps CPU
gpu_time = data["GPU_Time"]  # Temps GPU

# Tracer les graphes
plt.figure(figsize=(10, 6))

plt.plot(N_values, cpu_time, label='Temps CPU', marker='o')
plt.plot(N_values, gpu_time, label='Temps GPU', marker='x')

# Ajouter des labels et un titre
plt.xscale('log')
plt.yscale('log')
plt.xlabel('taille de N (log scale)')
plt.ylabel('Temps (seconds, log scale)')
plt.title('Temps d exécution en fct de N (GPU vs CPU)')
plt.legend()

# Afficher le graphique
plt.grid(True)
plt.show()
