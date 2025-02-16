import numpy as np

import time
from mpi4py import MPI


def isSorted(a):
    for i in range(1, len(a)):
        if a[i] < a[i - 1]:
            return False
    return True


def flatten(array):
    return np.array([x for a in array for x in a])


globCom = MPI.COMM_WORLD.Dup()
nbp, rank = globCom.size, globCom.rank

N = 2 * 3 * 4 * 100
N_loc = N // nbp

# 1 - générer le tableau à trier et l'envoyer à tout le monde
array = None
if rank == 0:
    array = np.random.randint(0, 100, N)
array_loc = np.zeros(N_loc, dtype=int)
globCom.Scatter(array, array_loc)

debut = time.time()

# 2 - trouver les quantiles pour tous les processus
quantiles_loc = np.zeros(nbp - 1)
for k in range(quantiles_loc.shape[0]):
    quantiles_loc[k] = np.quantile(array_loc, (k + 1) * (1 / nbp))
quantiles = np.zeros(quantiles_loc.shape)
globCom.Allreduce(quantiles_loc, quantiles, MPI.SUM)
quantiles /= nbp  # on prend la moyenne des quantiles locaux

# 3 - répartir les données locales dans les différentes buckets
buckets_loc = []
buckets_loc.append(array_loc[array_loc <= quantiles[0]])
for k in range(1, nbp - 1):
    buckets_loc.append(
        array_loc[
            np.logical_and(array_loc <= quantiles[k], array_loc > quantiles[k - 1])
        ]
    )
buckets_loc.append(array_loc[array_loc > quantiles[-1]])

# 4 - fusionner les buckets entre tous les processus
values_loc = None
for k in range(nbp):
    if k == rank:
        values_loc = globCom.gather(buckets_loc[k], root=k)
    else:
        globCom.gather(buckets_loc[k], root=k)
values_loc = flatten(values_loc)

print(
    "Bucket {} : taille {}\t({:.2f}%)".format(
        rank, len(values_loc), 100 * len(values_loc) / N
    )
)

# 5 - trier localement et rassembler le résultat
sortedValues_loc = np.sort(values_loc)
sortedArray = globCom.gather(sortedValues_loc, root=0)
fin = time.time()
if rank == 0:
    sortedArray = flatten(sortedArray)
    assert isSorted(sortedArray)
    assert np.all(np.sort(array) == sortedArray)
    print("Test ok")
    print("Temps total: {}".format(fin - debut))

exit()
