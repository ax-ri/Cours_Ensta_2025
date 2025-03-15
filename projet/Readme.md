# Rendu projet

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
