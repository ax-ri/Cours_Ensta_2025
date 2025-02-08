# TD 2

## Exercice 1

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

On réparti ainsi mieux les points qui nécessitent un long temps de calcul entre les différents noeuds de calcul. Voici les résultats de cette approche :

| nbp | Temps de calcul (s) | Speedup | Comparaison Q1 |
| --- | ------------------- | ------- | -------------- |
| 1   | 2.916               | 1       | 1              |
| 2   | 1.507               | 1.935   | 1.053          |
| 3   | 1.133               | 2.574   | 1.071          |
| 4   | 0.853               | 3.419   | 1.068          |

On a bien une meilleur performance qu'à la première question.