# Rendu projet

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
