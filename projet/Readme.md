# Rendu projet

## Partie 3

### Résultats

* Les temps d'exécution du pas de temps de la simulation issus de la synthèse des parties 1 et 2 (parallélisation avec OpenMP et distribution avec MPI) sont présentés ci-dessous. Ligne de commande : `OMP_NUM_THREADS=<X> mpiexec --bind-to none -n 2 simulation.exe -l 200 -n 500 --no-display`
  * _Note:_ L'argument `--no-display` est nécessaire pour ne pas utiliser la SDL sur le serveur `salle` (qui n'a pas d'environnement graphique). Cependant, dans ce mode, seul l'affichage de la fenêtre SDL est désactivé : les envois / réception avec MPI sont conservés, donc les mesures du temps d'affichage ont toujours un sens.
  
  | `OMP_NUM_THREADS` | Durée moyenne d'exécution du pas de temps (secondes) | Accélération |
  | ----------------- | ---------------------------------------------------- | ------------ |
  | 1                 | 0.0232557                                            | 1            |
  | 2                 | 0.0203033                                            | 1.145        |
  | 3                 | 0.0190324                                            | 1.222        |
  | 4                 | 0.0227191                                            | 1.024        |
  | 5                 | 0.0271036                                            | 0.858        |
  | 6                 | 0.0322608                                            | 0.721        |
  | 7                 | 0.0310944                                            | 0.748        |
  | 8                 | 0.0391599                                            | 0.594        |
  | 16                | 0.052267                                             | 0.445        |
  | 32                | 0.0683472                                            | 0.340        |

  ```mermaid
  xychart-beta
    title "Durée moyenne d'exécution du pas de temps"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Temps d'exécution (secondes)"
    bar [0.0232557, 0.0203033, 0.0190324, 0.0227191, 0.0271036, 0.0322608, 0.0310944, 0.0391599, 0.052267, 0.0683472]
  ```

  ```mermaid
  xychart-beta
    title "Accélération de l'exécution du pas de temps"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Accélération"
    line [1, 1.145, 1.222, 1.024, 0.858, 0.721, 0.748, 0.594, 0.445, 0.340]
   ```

  | `OMP_NUM_THREADS` | Durée moyenne d'exécution de l'affichage (secondes) | Accélération |
  | ----------------- | --------------------------------------------------- | ------------ |
  | 1                 | 0.0232423                                           | 1            |
  | 2                 | 0.0202916                                           | 1.145        |
  | 3                 | 0.0190215                                           | 1.222        |
  | 4                 | 0.022706                                            | 1.024        |
  | 5                 | 0.027088                                            | 0.858        |
  | 6                 | 0.0322793                                           | 0.720        |
  | 7                 | 0.0310765                                           | 0.748        |
  | 8                 | 0.0391374                                           | 0.594        |
  | 16                | 0.0522384                                           | 0.445        |
  | 32                | 0.0683054                                           | 0.340        |

  ```mermaid
  xychart-beta
    title "Durée moyenne d'exécution de l'affichage"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Temps d'exécution (secondes)"
    bar [0.0232423, 0.0202916, 0.0190215, 0.022706, 0.027088, 0.0322793, 0.0310765, 0.0391374, 0.0522384, 0.0683054]
  ```

  ```mermaid
  xychart-beta
    title "Accélération de l'affichage"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Accélération"
    line [1, 1.145, 1.222, 1.024, 0.858, 0.720, 0.748, 0.594, 0.445, 0.340]
  ```

### Interprétation

* De manière assez logique, l'évolution du temps d'exécution est similaire pour la partie affichage et pour la partie temps.
* Contrairement à la partie 1, ici on observe une configuration pour laquelle une accélération est possible. Cependant, quand le nombre de threads devient grand, on retrouve le comportement observé en partie 1 : un trop grand nombre de threads fait que le programme perd plus de temps à attendre que les différents threads se synchronisent qu'il n'en gagne en parallélisant. Combiner OpenMP et MPI a donc bien permis d'obtenir une accélération supérieure à 1.