/* ============================================================
 * initialisation.h - Interface du jeu de donnees de demonstration
 * ------------------------------------------------------------
 * ROLE : declare l'initialisation de la boutique, le chargement des
 *        donnees de test et la liberation complete de la memoire.
 * ============================================================ */
#ifndef INITIALISATION_H
#define INITIALISATION_H

#include "types.h"

/* Met la boutique a zero, alloue catalogue et repertoire clients.
 * Retourne 1 si succes, 0 si une allocation echoue. */
int  initialiserBoutique(Boutique *b);

/* Charge 8 produits, 5 clients et 6 commandes. Retourne 1 si succes. */
int  initialiserDonneesTest(Boutique *b);

/* Libere TOUS les niveaux (lignes, commandes, clients, produits). */
void detruireBoutique(Boutique *b);

#endif
