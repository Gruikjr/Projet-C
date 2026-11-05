/* ============================================================
 * commande.h - Interface du module Commandes
 * ------------------------------------------------------------
 * ROLE : creation des commandes, gestion des lignes, changement de
 *        statut, annulation et calcul des montants.
 * UTILISE PAR : main.c (menu), initialisation.c (jeu de test),
 *        statistique.c.
 * REGLES : stock decremente a l'ajout d'une ligne, reintegre a la
 *        suppression d'une ligne ou a l'annulation ; points de fidelite
 *        (1 par tranche de 10 EUR TTC) credites/retires en consequence.
 * ============================================================ */
#ifndef COMMANDE_H
#define COMMANDE_H

#include "types.h"

/* Codes de retour (succes = CMD_OK, erreurs negatives) */
enum {
    CMD_OK = 0,
    CMD_ERR_INTROUVABLE   = -1,  /* client ou commande inconnu(e)      */
    CMD_ERR_STATUT        = -2,  /* statut incompatible avec l'action  */
    CMD_ERR_PRODUIT       = -3,  /* produit inconnu ou desactive       */
    CMD_ERR_DOUBLON       = -4,  /* reference deja presente            */
    CMD_ERR_STOCK         = -5,  /* stock insuffisant                  */
    CMD_ERR_MEMOIRE       = -6,
    CMD_ERR_QUANTITE      = -7,
    CMD_ERR_DERNIERE_LIGNE = -8, /* une commande garde 1 ligne minimum */
    CMD_ERR_DATE          = -9
};

/* Message lisible pour un code d'erreur */
const char *messageCommande(int code);

/* Calcul des montants (sous-totaux, remise, TVA, port, TTC) */
void calculCommande(Commande *commande);

/* Creation d'une commande vide EN_COURS (adresse du client recopiee).
 * Retourne le numero (> 0) ou un code d'erreur negatif. */
int creerCommande(Boutique *b, int idClient, Date date);

/* Supprime une commande creee mais restee sans ligne (annule la creation) */
void abandonnerCommandeVide(Boutique *b, int idClient, int numero);

/* Gestion des lignes (commande EN_COURS uniquement) */
int ajouterLigne(Boutique *b, int idClient, int numero,
                 const char *reference, int quantite);
int supprimerLigne(Boutique *b, int idClient, int numero,
                   const char *reference);

/* Statut */
int changerStatutCommande(Boutique *b, int idClient, int numero,
                          const char *nouveauStatut);
int annulerCommande(Boutique *b, int idClient, int numero);

/* Recherche */
Commande *chercherCommande(Boutique *b, int idClient, int numero);
Commande *chercherCommandeParNumero(Boutique *b, int numero);

/* Affichage */
void afficherCommande(const Commande *commande);
void afficherCommandes(Boutique *b, int idClient);

#endif
