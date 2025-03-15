# Rendu projet

_Membres du binôme : Doris Diallo, Axel Richard_

## Partie 1

* La sortie de la commande `lscpu` pour les deux machines de test (laptop et serveur `salle.ensta.fr`), ainsi que les temps d'exécution de la version séquentielle, sont donnés ci-dessous :
  * Laptop
    ```
    $ lscpu
    Architecture:           x86_64
      CPU op-mode(s):       32-bit, 64-bit
    CPU(s):                 8
      On-line CPU(s) list:  0-7
    Thread(s) per core:     2
    Core(s) per socket:     4
    Socket(s):              1
    Caches (sum of all):      
        L1d:                192 KiB (4 instances)
        L1i:                128 KiB (4 instances)
        L2:                 5 MiB (4 instances)
        L3:                 8 MiB (1 instance)
    ```
    ```
    $ ./simulation.exe -l 200 -n 500
    Average time step duration: 0.0261982 seconds
    Average display step duration: 0.0157664 seconds
    ```
  *  Serveur `salle`
    ```
    $ lscpu
    Architecture :                          x86_64
    Mode(s) opératoire(s) des processeurs : 32-bit, 64-bit
    Processeur(s) :                         32
    Liste de processeur(s) en ligne :       0-31
    Thread(s) par cœur :                    2
    Cœur(s) par socket :                    8
    Socket(s) :                             2
    Cache L1d :                             32K
    Cache L1i :                             32K
    Cache L2 :                              256K
    Cache L3 :                              20480K
    ```
    ```
    $ ./simulation.exe -l 200 -n 500 --no-display
    Average time step duration: 0.0193751 seconds
    Average display step duration: 0 seconds
    ```

### Vérification de la correction après parallélisation

* Pour vérifier si la simulation obtenue est la même, on commence par _dumper_ le contenu des variables `fire_map` et `vegetal_map` toutes les 32 itérations dans des fichiers textes. Ensuite, pour vérifier que la simulation parallélisée donne le même résultat que sa version séquentielle, il suffit de comparer les variables avec leur dump original.
* Concrètement, il suffit de :
  * Lancer une première fois la simulation dans sa version **séquentielle** avec le flag `--dump` dans la ligne de commande. Cela génère des fichiers texte dans le dossier `src/dump/`.
  * Pour tester une version différente de la simulation, lancer la simulation avec le flag `--check`

### Résultats

* Après parallélisation avec OpenMP, la simulation obtenue n'est pas exactement la même : elle reste visuellement très semblable mais, malgré les différentes tentatives, la vérification exposée ci-dessus détecte des différences dans l'état des variables `fire_map` et `vegetal_map`. Cela s'explique par le fait que les itérations de la boucle `for` de la fonction `Model::update()` ne sont pas complètement indépendantes.
* Les résultats issus de l'exécution de la version parallélisée avec OpenMP pour différents nombre de threads sont donnés ci-dessous (ligne de commande : `simulation.exe -l 200 -n 500 --no-display`).
  
  | `OMP_NUM_THREADS` | Durée moyenne d'exécution du pas de temps (secondes) | Accélération |
  | ----------------- | ---------------------------------------------------- | ------------ |
  | 1                 | 0.0193751                                            | 1            |
  | 2                 | 0.0278041                                            | 0.697        |
  | 3                 | 0.0210733                                            | 0.919        |
  | 4                 | 0.0208295                                            | 0.930        |
  | 5                 | 0.0229433                                            | 0.844        |
  | 6                 | 0.0284129                                            | 0.682        |
  | 7                 | 0.0293252                                            | 0.661        |
  | 8                 | 0.0305744                                            | 0.634        |
  | 16                | 0.0557251                                            | 0.348        |
  | 32                | 0.0674951                                            | 0.287        |

  ```mermaid
  xychart-beta
    title "Durée moyenne d'exécution du pas de temps"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Temps d'exécution (secondes)"
    bar [0.0193751, 0.0278041, 0.0210733,  0.0208295, 0.0229433, 0.0284129, 0.0293252, 0.0305744, 0.0557251, 0.0674951]
  ```

    ```mermaid
  xychart-beta
    title "Accélération de l'exécution du pas de temps"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32]
    y-axis "Accélération"
    line [1, 0.697, 0.919, 0.930, 0.844, 0.682, 0.661, 0.634, 0.348, 0.287]
  ```

### Interprétation

Contrairement à ce que l'on pourrait espérer, plus le nombre de threads augmente, plus la simulation met de temps à s'exécuter. Cela peut s'expliquer par le fait que les écritures dans les itérations de la boucle nécessitent d'être réalisées séquentiellement (avec la construction `#pragma omp critical`) pour éviter les _race conditions_. On remarque aussi que les résultats sont légèrement meilleurs pour 3 et 4 threads, mais en aucun cas l'accélération ne dépasse 1.

## Partie 2

### Résultats

La variation de la durée de l'exécution du pas de temps (en secondes) est présentée ci-dessous.

| Nombre de noeuds MPI   | Temps d'exécution (secondes) | Accélération |
| ---------------------- | ---------------------------- | ------------ |
| 1 (programme original) | 0.0193751                    | 1            |
| 2                      | 0.020568                     | 0.942        |
| 3                      | 0.0208326                    | 0.930        |
| 4                      | 0.0212218                    | 0.913        |
| 5                      | 0.0251048                    | 0.772        |
| 6                      | 0.0208846                    | 0.928        |
| 7                      | 0.0209114                    | 0.927        |
| 8                      | 0.0253614                    | 0.764        |
| 16                     | 0.0253987                    | 0.763        |
| 32                     | 0.0258514                    | 0.749        |
| 64                     | 0.0270222                    | 0.717        |

  ```mermaid
  xychart-beta
    title "Durée moyenne d'exécution"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64]
    y-axis "Temps d'exécution (secondes)"
    bar [0.0193751, 0.020568, 0.0208326, 0.0212218, 0.0251048, 0.0208846, 0.0209114, 0.0253614,  0.0253987, 0.0258514, 0.0270222]
  ```

  ```mermaid
  xychart-beta
    title "Accélération de l'exécution"
    x-axis "Nombre de threads" [1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64]
    y-axis "Accélération"
    line [1, 0.942, 0.930, 0.913, 0.772, 0.928, 0.927, 0.764, 0.763, 0.749, 0.717]
   ```

### Interprétation

On remarque que l'amélioration du temps n'est pas très significative. Cela est probablement dû aux ralentissement introduits par la communication inter-processus (envois et réceptions avec `MPI`).

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

## Partie 4

### Explication de la démarche

Comme pour la partie 2, l'affichage se fait sur le processus 0. Le calcul est ensuite réparti sur tous les autres processus.  
Pour cela, on commence par définir un groupe de communication pour les processus de calculs.  

```cpp
  int color = (rank == 0) ? MPI_UNDEFINED : 0;
  MPI_Comm calcComm;
  MPI_Comm_split(globComm, color, rank, &calcComm);
```

Étant donné que le calcul de l'avancement nécessite l'utilisation des cellules adjacentes, il faut utiliser des cellules fantômes. Chaque processeur de calcul se charge de $\frac{n_{cell}}{n_{proc\_calcul}}$ lignes. Il faut ajouter une ligne de cellules fantômes pour le premier et le derniers processus, et deux pour tous les autres. Chaque processus effectue sa propre simulation en local. Après chaque itération, il faut actualiser toutes les cellules fantômes. On rassemble ensuite toutes les données sur le premier processus de calcul, qui se charge ensuite d'envoyer les cartes de feu et de végétation au processus d'affichage.

### Tentative d'explication des problèmes

Malgré plusieurs tentative de débogage, nous n'avons pas réussi à mettre en oeuvre cettre partie. 

- Des problèmes d'indiçage au niveau du GatherV.
- Il n'est pas impossible que le GatherV se fasse "dans le sens des colonnes" plutôt que dans le "sens des lignes". Cependant, ce problème devrait donner de faux résultats, par des erreurs de Malloc

Au-delà de l'erreur fatale, de nombreuses optimisations manquent à ce programme (communication asynchrone, ...). Cependant, étant donné que la version basique ne fonctionne pas, il est compliqué de mettre en place ces optimisations.