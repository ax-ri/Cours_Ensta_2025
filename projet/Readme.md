# Rendu projet

## Partie 1

* La sortie de la commande `lscpu` pour les deux machines de test (laptop et serveur `salle.ensta.fr`), ainsi que les temps d'exécution de la version séquentielle, sont donnés ci-dessous :
  * Laptop
    ```bash
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
    ```bash
    $ ./simulation.exe -l 200 -n 500
    Average time step duration: 0.0261982 seconds
    Average display step duration: 0.0157664 seconds
    ```
  *  Serveur `salle`
    ```bash
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
    ```bash
    $ simulation.exe -l 200 -n 500 --no-display
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