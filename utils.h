/* ============================================================
 * utils.h - Interface du module Saisie & validation
 * ------------------------------------------------------------
 * ROLE : declare les fonctions utilitaires partagees : saisie securisee
 *        (chaine, entier, reel) et petits outils sur les chaines.
 * UTILISE PAR : tous les modules qui interagissent avec l'utilisateur.
 * ============================================================ */
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Saisie securisee (fgets + conversion, boucle jusqu'a saisie valide) */
void   lireChaine(const char *invite, char *dest, size_t taille);
int    lireEntier(const char *invite, int min, int max);
double lireReel(const char *invite, double min, int minExclus);

/* Outils sur les chaines */
void versMinuscules(char *dest, const char *src, size_t taille);
int  contientMotCle(const char *texte, const char *mot);

#endif
