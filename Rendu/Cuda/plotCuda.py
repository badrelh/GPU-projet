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

plt.plot(N_values, cpu_time, label='CPU Time', marker='o')
plt.plot(N_values, gpu_time, label='GPU Kernel Time', marker='x')

# Ajouter des labels et un titre
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Size of N (log scale)')
plt.ylabel('Time (seconds, log scale)')
plt.title('Time vs N for CPU and GPU Operations')
plt.legend()

# Afficher le graphique
plt.grid(True)
plt.show()
