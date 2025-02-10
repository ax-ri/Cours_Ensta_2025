# TD 2

## Exercice 1 - Parallélisation ensemble de Mandelbrot

_Les fichiers sources de cet exercices sont nommés `mandelbrot-mpi-<k>.py` avec $k$ le numéro de la question._

### Question 1

| nbp | Temps de calcul (s) | Speedup |
| --- | ------------------- | ------- |
| 1   | 2.905               | 1       |
| 2   | 1.587               | 1.830   |
| 3   | 1.213               | 2.395   |
| 4   | 0.911               | 3.189   |

*Remarque* : pour utiliser la fonction `Gather()` de MPI, il a été nécessaire de modifier la façon dont est stockée l'image, en stockant les pixels dans l'ordre `(y, x)` plutôt que `(x, y)`. En effet, puisque la découpe des blocks des pixels à calculer est faite verticalement, il faut la première coordonnée corresponde à celle de la découpe pour que `Gather()` fonctionne correctement.

### Question 2

Plutôt que de découper l'image en *nbp* blocks de lignes côtes-à-côtes, on prend une ligne tous les *nbp*, comme montré sur le schéma ci-dessous :
```
Découpage Q1     | Découpage Q2
00000000         | 00000000
00000000         | 11111111
00000000         | 22222222
11111111         | 00000000
11111111         | 11111111
11111111         | 22222222
22222222         | 00000000
22222222         | 11111111
22222222         | 22222222
```

On réparti ainsi mieux les points qui nécessitent un long temps de calcul entre les différents nœuds de calcul. Voici les résultats de cette approche :

| nbp | Temps de calcul (s) | Speedup | Comparaison Q1 ($t_{Q1} / t_{Q2}$) |
| --- | ------------------- | ------- | ---------------------------------- |
| 1   | 2.916               | 1       | 1                                  |
| 2   | 1.507               | 1.935   | 1.053                              |
| 3   | 1.133               | 2.574   | 1.071                              |
| 4   | 0.853               | 3.419   | 1.068                              |

On a bien une meilleur performance qu'à la première question.

### Question 3

On met en place la stratégie maître-esclave suivante : le nœud 0 découpe l'image en `nbp - 1` blocks en donne un à chacun des autres nœuds, qui lui renvoient ensuite leur résultat pour qu'il l'affiche.

| nbp | Temps de calcul (s) | Speedup | Comparaison Q2 ($t_{Q2} / t_{Q3}$) |
| --- | ------------------- | ------- | ---------------------------------- |
| 2   | 2.946               | 1       | 1                                  |
| 3   | 1.531               | 1.924   | 0.984                              |
| 4   | 1.071               | 2.750   | 1.058                              |

Les performances sont équivalentes à la Q2 (il faudrait plus de nœuds pour comparer, mais l'ordinateur utilisé pour les tests n'a que 4 cœurs disponibles au maximum). Cela étant dit, cette approche a l'avantage de mener à un code plus structuré entre processus maître et processus esclave.

## Exercice 2 - Produit matrice-vecteur

Pour tout l'exercice, on choisit $N = 3600$, qui est divisible par 2, 3 et 4, ainsi on pourra tester avec toutes les valeurs de `nbp` possibles.

### Question 1

On a $N_{loc} = N / nbp$. On obtient les résultats suivants :

| nbp | Temps de calcul (s) | Speedup |
| --- | ------------------- | ------- |
| 1   | 0.0042596           | 1       |
| 2   | 0.0019298           | 2.208   |
| 3   | 0.0021222           | 2.007   |
| 4   | 0.0020132           | 2.116   |

### Question 2

On obtient les résultats suivants :

| nbp | Temps de calcul (s) | Speedup |
| --- | ------------------- | ------- |
| 1   | 0.0050313           | 1       |
| 2   | 0.0020456           | 2.460   |
| 3   | 0.0080280           | 0.627   |
| 4   | 0.0113051           | 0.445   |

## Exercice 3 - Entraînement pour l'examen écrit

1. > En utilisant la loi d’Amdhal, pouvez-vous prédire l’accélération maximale que pourra obtenir Alice avec son code (en considérant n ≫ 1) ?

La loi d'Amdhal s'écrit $S(n) = \frac{t_s}{ft_s + \frac{(1-f)t_s}{n}}$, soit pour $n \rightarrow \infty$ : $S(n) = \frac{1}{f}$. Ici $f = 0.1$ donc l'accélération maximale vaut $\frac{1}{0.1} = 10$.

2. > À votre avis, pour ce jeu de donné spécifique, quel nombre de nœuds de calcul semble-t-il raisonnable de prendre pour ne pas trop gaspiller de ressources CPU ?
L'accélération maximale (cf Q1) est de 10 donc il est inutile de prendre plus de 10 nœuds de calcul.

3. > En effectuant son calcul sur son calculateur, Alice s’aperçoit qu’elle obtient une accélération maximale de quatre en augmentant le nombre de nœuds de calcul pour son jeu spécifique de données. En doublant la quantité de donnée à traiter, et en supposant la complexité de l’algorithme parallèle linéaire, quelle accélération maximale peut espérer Alice en utilisant la loi de Gustafson ?    

Alice n'obtient une accélération que de 4 donc en fait $f = 0.25$. On applique alors la loi de Gustafson avec $t_s=0.25$ et $t_p=0.75$ et on obtient $S(n) = n + 0.25(1-n) = 0.25 + 0.75n$ d'où $S(n) = 1.75$. 
Ainsi, Alice peut espérer une accélération de 1.75 en doublant le nombre de nœuds de calcul, .