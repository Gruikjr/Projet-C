/* ============================================================
 * produit.h - Interface du module Catalogue produits
 * ------------------------------------------------------------
 * ROLE : declare le CRUD des produits (ajouter, modifier, supprimer,
 *        lister, rechercher), la recherche par reference et les alertes
 *        de stock.
 * UTILISE PAR : main.c (menu), commande.c (lecture/mise a jour du stock),
 *        statistique.c (palmares produits).
 * ============================================================ */
#ifndef PRODUIT_H
#define PRODUIT_H

#include "types.h"
#include <stddef.h>

/* Codes de retour */
enum {
    PRODUIT_OK = 0,
    PRODUIT_DOUBLON = -1,
    PRODUIT_MEMOIRE = -2,
    PRODUIT_STOCK_INSUFFISANT = -3
};

/* Recherche */
int indexProduit(const Boutique *b, const char *reference);
Produit *trouverProduitActif(Boutique *b, const char *reference);

/* Gestion du catalogue */
int ajouterProduitDonnees(Boutique *b, const Produit *p);
void ajouterProduit(Boutique *b);
void modifierProduit(Boutique *b);
void supprimerProduit(Boutique *b);
void afficherCatalogue(const Boutique *b);
void rechercherProduit(const Boutique *b);
void afficherAlertesStock(const Boutique *b);

/* Noms conservés pour compatibilité avec l'ancienne version du module */
void mettreAJourProduit(Boutique *b);
void afficherProduits(const Boutique *b);

/* Gestion du stock */
int retirerStock(Produit *p, int quantite);
void reintegrerStock(Produit *p, int quantite);

#endif
