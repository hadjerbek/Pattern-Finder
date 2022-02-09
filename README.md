% IN405 TD11 -- Systèmes d'exploitation  
% Pattern Finder  

# AUTEUR  
Bekhira Hadjer  

# Contenu du projet  
Ce projet est constituée des fichiers suivants :  
  
- README.md : fichier décrivant le contenu du projet ;  
- lorem.txt : fichier test dans lequel des motifs peuvent être recherchés ;  
- test_basic.sh : fichier contenant des scénario de test basique reflétant  
  une partie des fonctionnalités du programme.  
- Makefile : matériel de compilation, permet la compilation du programme et le  
  lancement d'un test prédéfini.  
- occurences.c et occurences.h : code source de la structure de liste chainée  
  permettant de stocker les indices d'occurrence du pattern à chercher  
- pattern-finder.c : code source et fonction principale du programme permettant  
  la recherche de pattern  
 
  
# Utilisation  
La compilation du programme se fait simplement à l'aide de la commande make :  
```  
$ make  
```  
  
Cette commande compile le programme et lance un exemple d'exécution. L'exemple  
testée est le suivant :  
```  
$ ./pattern-finder 2 lor lorem.txt  
```   
(recherche du pattern lor dans le fichier lorem.txt en utilisant 2 threads).  
  
L'exécution, comme décrite le 'usage()',  
se fait comme suit :  
  
./pattern-finder [n_workers] [pattern] [file]  
  Avec : n_workers  - le nombre de thread à utiliser  
         pattern    - le pattern à rechercher  
         file       - Le fichier où rechercher le pattern  
  
Exple :  
```bash  
$ ./pattern-finder 8 lor lorem.txt  
$ ./pattern-finder 2 lorem lorem.txt  
$ ./pattern-finder 1 Lorem lorem.txt  
$ ./pattern-finder 4 not_occured lorem.txt  
```  
  
Une fois exécutée, le programme affiche les indices où apparaissent le pattern  
recherché dans le fichier. Si le pattern n'apparait pas dans le fichier, aucun  
indice n'est affiché.  
Si un nombre de paramètre insuffisant est passé en argument ou que l'un  
des arguments est incorrect, le programme affiche l'aide à l'utilisation.  
  
  
# FONCTIONNEMENT DU PROGRAMME  
Le programme recherche dans le fichier donné en paramètre les occurrences du  
pattern donné en paramètre de façon concurrente à l'aide du nombre de thread  
donné en paramètre.  
Pour cela, le programme découpe le fichier de façon équitable entre les threads  
et chaque thread recherche dans sa partie du fichier les occurrences du pattern.  
Si un thread détecte une occurrence du pattern dans sa partie, il l'ajoute à la  
liste chainée contenant le résultat du programme. Une fois tous les threads  
terminés, le thread principal affiche le résultat sur le terminal. Le résultat  
du programme est donc l'affichage de l'indice du premier caractère de chaque  
occurrence du pattern dans le fichier.  
Nous avons ajouté des scénarios de tests au fichier test_basic.sh (recherche de  
pattern supplémentaire, recherche avec 1 jusqu'à 10 threads).  
  
