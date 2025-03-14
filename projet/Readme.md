# Rendu projet

## Partie 1

* Sur la machine `salle.ensta.fr`
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
* _(à exécuter sur salle pour avoir les délais)_

### Vérification de la correction après parallélisation

* Pour vérifier si la simulation obtenue est la même, on commence par _dumper_ le contenu des variables `fire_map` et `vegetal_map` toutes les 32 itérations dans des fichiers textes. Ensuite, pour vérifier que la simulation parallélisée donne le même résultat que sa version séquentielle, il suffit de comparer les variables avec leur dump original.
* Concrètement, il suffit de :
  * Lancer une première fois la simulation dans sa version **séquentielle** avec le flag `--dump` dans la ligne de commande. Cela génère des fichiers texte dans le dossier `src/dump/`.
  * Pour tester une version différente de la simulation, lancer la simulation avec le flag `--check`

### Résultats

* Après parallélisation avec OpenMP, la simulation obtenue n'est pas exactement la même : elle reste visuellement très semblable mais, malgré les différentes tentatives, la vérification exposée ci-dessus détecte des différences dans l'état des variables `fire_map` et `vegetal_map`. Cela s'explique par le fait que les itérations de la boucle `for` de la fonction `Model::update()` ne sont pas complètement indépendantes.
* 