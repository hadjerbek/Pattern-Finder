//
//  occurences.h
//
//  Created by Hadjer Bekhira on 25/05/2021.
//

#ifndef numlines_h
#define numlines_h

#include <stdlib.h>
#include <stdio.h>

/**
 * La liste des indices d'occurences d'un pattern
 * nous avons opté pour une liste chainée car le nombre d'occurences d'un pattern
 * n'est pas connu à l'avance
*/

/**
 * element représente les maillons (noeuds) de la liste
 */
struct element_s
{
    int ind_occurence;
    struct element_s *suivant;
};
typedef struct element_s* element;

/**
* Liste chaînée d'entiers pour stocker les indices d'occurences d'un pattern
*/
struct liste_chainee_entier_s
{
  element premier;
};
typedef struct liste_chainee_entier_s* liste_chainee_entier;


/**
 * Crée un element (maillon) de la liste
 * @param ind_occurence l'indice d'occurences du pattern dans le texte
 * @return l'element créé
 */
element creer_element(int ind_occurence);

/**
 * Librer la mémoire allouée pour un noeud de la liste
 * @param e l'adresse de l'element à désallouer
 */
void free_element(element e);

/**
 * Crée une liste vide simplement chaînée pour le stcokage des indices d'occurences
 * @return la liste chaînée
 */
liste_chainee_entier creer_liste(void);

/**
 * Teste si une liste est vide. La liste est vide si le premier element est nul
 * @param l la liste a tester
 * @return 1 si la liste est vide, 0 sinon
 */
int liste_est_vide(liste_chainee_entier l);

/**
 * Ajoute un element à la fin d'une liste
 * @param l la liste chaînée
 * @param num_ligne la valeur à ajouter
 */
void liste_ajouter(liste_chainee_entier l, int ind_occurence);

/**
 * Detruit l'espace mémoire alloué pour une liste chaînée
 * @param l l'adresse à désallouer
 */
void liste_detruire(liste_chainee_entier l);

/**
 * Affiche une liste simplement chaînée d'entiers
 * @param l la liste à afficher
 */
void affiche_liste(liste_chainee_entier l);

#endif /* numlines_h */
