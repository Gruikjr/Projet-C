/* ============================================================
 * utils.h - Interface du module Saisie & validation
 * ------------------------------------------------------------
 * ROLE : saisie securisee (chaine, entier, reel, date, champs valides)
 *        et petits outils sur les chaines.
 * UTILISE PAR : tous les modules qui interagissent avec l'utilisateur.
 * ============================================================ */
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "types.h"

/* Saisie securisee (fgets + conversion, boucle jusqu'a saisie valide) */
void   lireChaine(const char *invite, char *dest, size_t taille);
int    lireEntier(const char *invite, int min, int max);
double lireReel(const char *invite, double min, int minExclus);
Date   lireDate(const char *invite);

/* Saisie d'une chaine acceptee par la fonction valide() */
void lireChaineValide(const char *invite, char *dest, size_t taille,
                      int (*valide)(const char *), const char *messageErreur);

/* Validations (retournent 1 si correct, 0 sinon) */
int estEmailValide(const char *email);
int estTelephoneValide(const char *tel);      /* 10 chiffres */
int estCodePostalValide(const char *cp);      /* 5 chiffres  */
int dateValide(int jour, int mois, int annee); /* bissextile gere */

/* Outils sur les chaines */
void versMinuscules(char *dest, const char *src, size_t taille);
int  contientMotCle(const char *texte, const char *mot);

#endif
