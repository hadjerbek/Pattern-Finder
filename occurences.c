//
//  occurences.c
//
//  Created by Hadjer Bekhira on 25/05/2021.
//

#include "occurences.h"

/**
 * Crée un element (maillon) de la liste
 * @param num_ligne numero de ligne où apparait le mot
 * @return l'element créé
 */
element creer_element(int ind_occurence){
    element res = (element)malloc(sizeof(struct element_s));
    if(res == NULL) {
        perror("Echec de l'allocation\n");
        exit(1);
    }
    res->ind_occurence = ind_occurence;
    res->suivant = NULL;
    return res;
}

/**
 * Librer la mémoire allouée pour un noeud de la liste
 * @param e l'adresse de l'element à désallouer
 */
void free_element(element e){
    free(e);
}

/**
 * Crée une liste vide simplement chaînée pour le stcokage des indices d'occurences
 * @return la liste chaînée
 */
liste_chainee_entier creer_liste(void)
{
    liste_chainee_entier liste = (liste_chainee_entier)
                                malloc(sizeof(struct liste_chainee_entier_s));
    if(liste == NULL) {
        perror("Echec de l'allocation\n");
        exit(1);
    }
    liste->premier = NULL;
    return liste;
}

/**
 * Ajoute un element à la liste de sorte que la liste reste triée en ordre
 * croissant
 * @param l la liste chaînée
 * @param num_ligne la valeur à ajouter
 */
void liste_ajouter(liste_chainee_entier l, int ind_occurence)
{
    element e = creer_element(ind_occurence);
    // La place où doit être insérée le nouvel élément
    element temp;
    // Cas de la liste vide
    if (l->premier == NULL) {
        // On ajoute l'element en tant que premier
        l->premier=e;
    }
    // Comparaison avec le premier element de la liste
    else if(ind_occurence < l->premier->ind_occurence)
    {
      e->suivant = l->premier;
      l->premier = e;
    } else { // Comparaison avec les autres éléments
      temp = l->premier;
      while (temp->suivant != NULL) {
        if(ind_occurence < temp->suivant->ind_occurence)
          // On a le bon endroit où inserer l'élément
          break;
        temp = temp->suivant;
      }
      e->suivant = temp->suivant;
      temp->suivant = e;
  }
}

/**
 * Detruit l'espace mémoire alloué pour une liste chaînée
 * @param l l'adresse à désallouer
 */
void liste_detruire(liste_chainee_entier l){
    // Récupère le premier element de la liste dans e
    element e = l->premier;
    element suiv = l->premier;
    while (e!=NULL) // Tant que la liste n'est pas vide
    {
        // On recupère l'element suivant
        suiv = e->suivant;
        // Puis on supprime l'element e actuel
        free_element(e);
        // e devient l'element suivant
        e = suiv;
    }
    // free la liste
    free(l);
}

/**
 * Affiche une liste simplement chaînée d'entiers
 * @param l la liste à afficher
 */
void affiche_liste(liste_chainee_entier l)
{
    // Récupère le premier element de la liste dans e
    element e = l->premier;
    while (e != NULL) // Tant que la liste n'est pas vide
    {
        // Affiche la valeur stockée dans e
        printf("%d\n", e->ind_occurence);
        // e reçoit l'element suivant
        e = e->suivant;
    }
    printf("\n");
}
