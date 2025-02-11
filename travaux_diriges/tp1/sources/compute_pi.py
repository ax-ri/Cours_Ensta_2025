# Calcul pi par une méthode stochastique (convergence très lente !)
import time
import numpy as np
from mpi4py import MPI

globCom = MPI.COMM_WORLD.Dup()
nbp, rank = globCom.size, globCom.rank

np.random.seed(rank)

output = open(f"Output{rank:03d}.txt", mode="w")

# Nombre d'échantillons :
nbSamples = 80_000_000

totalLocalSamples = nbSamples // nbp
remainingSamples = nbSamples % nbp
if rank < remainingSamples:
    totalLocalSamples += 1

beg = time.time()
# Tirage des points (x,y) tirés dans un carré [-1;1] x [-1; 1]
x = 2.0 * np.random.random_sample((totalLocalSamples,)) - 1.0
y = 2.0 * np.random.random_sample((totalLocalSamples,)) - 1.0
# Création masque pour les points dans le cercle unité
filtre = np.array(x * x + y * y < 1.0)
# Compte le nombre de points dans le cercle unité
sum = np.add.reduce(filtre, 0)

approxLoc = np.array([4.0 * sum / nbSamples], dtype=np.double)
approxGLob = np.zeros(1, dtype=np.double)
globCom.Allreduce(approxLoc, approxGLob, MPI.SUM)
end = time.time()

print(f"Temps pour calculer pi : {end - beg} secondes")
print(f"Pi vaut environ {approxGLob[0]} (local: {approxLoc[0]})")
