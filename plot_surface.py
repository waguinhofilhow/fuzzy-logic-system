import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Carregar dados
data = np.loadtxt("superficie.csv", delimiter=",")
comida = data[:, 0]
servico = data[:, 1]
gorjeta = data[:, 2]

# Reorganizar para grade
n = int(np.sqrt(len(gorjeta)))
X = comida.reshape((n, n))
Y = servico.reshape((n, n))
Z = gorjeta.reshape((n, n))

# Plot 3D
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')
surf = ax.plot_surface(X, Y, Z, cmap='viridis', edgecolor='none')
ax.set_title('Superfície Fuzzy da Gorjeta')
ax.set_xlabel('Qualidade da Comida')
ax.set_ylabel('Qualidade do Serviço')
ax.set_zlabel('Gorjeta (%)')
fig.colorbar(surf, shrink=0.5, aspect=5)
plt.show()
