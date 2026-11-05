/* ============================================================
 * statistique.h - Interface du module Statistiques
 * ------------------------------------------------------------
 * ROLE : declare le chiffre d'affaires (total, par mois, par annee) et
 *        les deux palmares (top 5 produits, classement clients).
 * NB : les commandes ANNULEE sont toujours exclues des calculs.
 * ============================================================ */
#ifndef STATISTIQUE_H
#define STATISTIQUE_H

#include "types.h"

/* Taille du palmares produits */
#define TAILLE_PALMARES_PRODUITS 5

/* Chiffre d'affaires (hors commandes ANNULEE) :
 * mois = 0 -> toute l'annee ; annee = 0 -> toute la periode.
 * Retourne le CA HT (apres remise) et, si ttc != NULL, le CA TTC. */
double calculerChiffreAffaires(const Boutique *b, int mois, int annee,
                               double *ttc);

/* Affichages (interactifs : interrogent l'utilisateur si besoin) */
void afficherChiffreAffaires(const Boutique *b);     /* global + periode  */
void afficherPalmaresProduits(const Boutique *b);    /* top 5 par quantite */
void afficherPalmaresClients(const Boutique *b);     /* tous, par CA       */

/* --- Fonctionnalites creatives --- */
/* Statistiques par categorie : quantites vendues et CA HT brut */
void afficherStatsCategories(const Boutique *b);
/* Export CSV de toutes les commandes (separateur ';'). Retourne 1 si OK. */
int  exporterCommandesCSV(const Boutique *b, const char *nomFichier);

#endif