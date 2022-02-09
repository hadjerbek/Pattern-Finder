#include "occurences.h"

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>

/**
* Structure contenant les arguments à passer à la fonction de thread search_pattern
*/
struct search_pattern_args
{
  long int nb_threads;       // Nombre total de threads
  int thread_id;        // L'id du thread courant
  liste_chainee_entier occurences_list; // Liste chaînée devrant contenir les
                                        // indices des occurences
  char *pattern;        // Le pattern à rechercher
  char *file_name;     // Nom du fichier dans lequel rechercher le pattern
  pthread_mutex_t *mutex; // Pour l'exclusion mutuelle des threads lors des sections critiques
  int result;         // Pour stocker le résultat de la fonction
};
typedef struct search_pattern_args* search_pattern_args;

/**
* Crée la structure contenant les arguments de la fonction search_pattern
*/
search_pattern_args create_args(long int nbt, int tid, liste_chainee_entier liste,
                                char *pattern, char *fname, pthread_mutex_t *mutex)
{
  search_pattern_args res = (search_pattern_args)
                          malloc(sizeof(struct search_pattern_args));
  if(!res)
  {
    fprintf(stderr, "[ERR] Failed to alloc memory : %s\n", strerror(errno));
    return NULL;
  }
  res->nb_threads = nbt;
  res->thread_id = tid;
  res->occurences_list = liste;
  // Il n'est pas nécessaire de faire une copie des chaines de caractères (strcpy)
  // car elles ne seront accédées qu'en lecture
  res->pattern = pattern;
  res->file_name = fname;
  res->mutex = mutex;

  return res;
}

/**
* Fonction de thread : rechercher un pattern dans un fichier
*/
void* search_pattern(void *args)
{
  // On récupère les Arguments
  search_pattern_args thread_args = (search_pattern_args) args;

  // Chaque thread crée son propre file descriptor. Cela permet aux threads de
  // déplacer la tête de lecture du fichier (file offset) et de lire dans le
  // fichier sans se préocuper des autres threads (évite les blocages lors des
  // manipulations du fichier). De plus, tous les threads ouvrent le fichier en
  // mode lecture seul. On n'a donc pas de problème d'accès concurrent.
  int fd = open(thread_args->file_name, O_RDONLY, 0600);
  if (fd == -1) {
      fprintf(stderr, "[ERR tid %d] Failed to open file: %s %s\n",
              thread_args->thread_id, thread_args->file_name, strerror(errno));
      thread_args->result = 1;
      return NULL;
  }

  // On compte le nombre de caractère du fichier (i.e taille du fichier) :
  // On se déplace à la fin du fichier et on récupère le décalage
  // par rapport au debut
  off_t file_size = lseek (fd, 0, SEEK_END);
  // renvoie 1 si erreur
  if(file_size == -1) {
    fprintf(stderr, "[ERR tid %d] Failed to get file size : %s\n",
            thread_args->thread_id, strerror(errno));
    thread_args->result = 1;
    return NULL;
  }

  char* indptr; // Pointeur vers l'occurence courant du pattern
  int position; // Position (indice) du premier caractère de
                // l'occurence courant dans le texte

  // Allocation de la mémoire pour stocker le texte
  char* file_text = (char*)malloc(sizeof(char) * file_size);
  if(!file_text)
  {
    fprintf(stderr, "[ERR tid %d] Failed to alloc memory : %s\n",
            thread_args->thread_id, strerror(errno));
    thread_args->result = 1;
    return NULL;
  }

  // Taille du pattern
  int pattern_len = strlen(thread_args->pattern);

  // On replace le curseur au début du fichier
  if(lseek (fd, 0, SEEK_SET) == -1) {
    fprintf(stderr, "[ERR tid %d] Failed to move file seek : %s\n",
            thread_args->thread_id, strerror(errno));
    thread_args->result = 1;
    free(file_text);
    return NULL;
  }

  // On recupère le texte
  if(read(fd, file_text, file_size) < 0)
  {
    fprintf(stderr, "[ERR tid %d] Failed to read file : %s\n",
            thread_args->thread_id, strerror(errno));
    thread_args->result = 1;
    free(file_text);
    return NULL;
  }

  // La portion de texte que doit traiter chaque thread
  int thread_portion = file_size/thread_args->nb_threads;
  // Chaque thread traite une portion de texte dédiée. La partie du fichier
  // à traiter pour chaque thread est de thread_id*thread_portion à
  // (thread_id+1)*thread_portion.
  int beg_portion = thread_args->thread_id*thread_portion;
  int end_portion = (thread_args->thread_id+1)*thread_portion;
  // Toutefois, pour éviter que des threads se retrouvent avec un mot découpé
  // (entre deux threads), on s'assure que les mots de debut et fin des threads
  // sont des mots entiers
  while (isalnum(file_text[end_portion-1]) != 0) {
    // On décale le caractère de fin jusqu'à obtenir un mot
    // (un caractère non alphanumérique)
    end_portion++;
  }
  while (thread_args->thread_id != 0 && beg_portion > 0
          && isalnum(file_text[beg_portion]) != 0
          && isalnum(file_text[beg_portion-1]) != 0) {
    // On décale le caractère de début jusqu'à ignorer le mot
    // (déjà pris en compte par le thread tid-1)
    beg_portion++;
  }

  // On ignore la partie du texte hors du champ de traitement du thread
  file_text[end_portion] = '\0';

  // Recherche du pattern dans le texte avec strstr :
  indptr = strstr(file_text+beg_portion, thread_args->pattern);

  // Tant que le pattern apparait dans le texte
  while (indptr) {
    // On recupère l'indice du premier caractère de l'occurence courant
    position = indptr - file_text;
    // On l'ajoute à la liste chainée d'occurence :
    // La liste est une variable partagée entre les threads, l'écriture dans
    // cette liste représente donc une section critique
    pthread_mutex_lock(thread_args->mutex);
    liste_ajouter(thread_args->occurences_list, position);
    pthread_mutex_unlock(thread_args->mutex);
    // indptr contient le texte à partie de l'occurence courante de pattern.
    // Pour la recherche suivante, on ignore le pattern qu'on vient de traiter
    indptr = strstr(indptr+pattern_len, thread_args->pattern);
  }

  // Libération de la mémoire et fermeture du fichier
  free(file_text);

  if(close(fd) < 0)
  {
    fprintf(stderr, "[ERR tid %d] Failed to close file : %s\n",
            thread_args->thread_id, strerror(errno));
    thread_args->result = 1;
    return NULL;
  }

  thread_args->result = 0;

  return NULL;
}


int usage(char *exec_name)
{
  fprintf(stderr, "\nUsage : \n"
                  "%s [n_workers] [pattern] [file]\n"
                  "With : n_workers - the number of thread to use\n"
                  "       pattern   - the pattern to search\n"
                  "       file      - path to the file where to search\n",
          exec_name);
  return 1;
}

int main(int argc, char** argv)
{

  if(argc < 4) return usage(argv[0]);

  // Conversion de l'arg du nombre dr thread en entier
  long int nbt = strtol(argv[1], NULL, 10);
  if(nbt == 0)
  {
    fprintf(stderr, "[ERR] 2nd argument '%s' is not integer or is null\n", argv[1]);
    return usage(argv[0]);
  }

  // Initialisation du mutex
  pthread_mutex_t mutex;
  if(pthread_mutex_init(&mutex, NULL))
  {
      fprintf(stderr, "[ERR] Mutex init failed\n");
      return 1;
  }

  // Création du tableau de threads
  pthread_t *tid = (pthread_t*)malloc(sizeof(pthread_t) * nbt);
  if(!tid)
  {
    fprintf(stderr, "[ERR] Failed to alloc memory : %s\n", strerror(errno));
    return 1;
  }

  // Création du tableau des arguments
  search_pattern_args *targs = (search_pattern_args*)
                                malloc(sizeof(search_pattern_args) * nbt);
  if(!targs)
  {
    fprintf(stderr, "[ERR] Failed to alloc memory : %s\n", strerror(errno));
    free(tid);
    return 1;
  }

  // Création de liste contenant le résultat
  liste_chainee_entier occurences_list = creer_liste();

  for(int i=0; i<nbt; i++)
  {
    // Initialisation des arguments et création des threads
    targs[i] = create_args(nbt, i, occurences_list, argv[2], argv[3], &mutex);
    pthread_create(tid+i, NULL, search_pattern, targs[i]);
  }

  // Attente des threads
  for(int i=0; i<nbt; i++)
  {
    pthread_join(tid[i], NULL);
  }

  int ret = 0;

  // Vérification des valeurs de retour (tous les threads ont-ils bien terminé ?)
  for(int i=0; i<nbt; i++)
  {
    if(targs[i]->result)
    {
      fprintf(stderr, "[ERR] An error occcured with thread %d\n", i);
      ret = 1;
    }
  }
  if(!ret) {
    // Affiche le résultat
    affiche_liste(occurences_list);
    ret = 0;
  }

  // Libération de la mémoire
  free(tid);
  for(int i=0; i<nbt; i++) free(targs[i]);
  free(targs);
  liste_detruire(occurences_list);
  pthread_mutex_destroy(&mutex);

  return ret;
}
