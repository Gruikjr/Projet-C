/* ============================================================
 * produit.h - Interface du module Catalogue produits
 * ------------------------------------------------------------
 * ROLE : declare le CRUD des produits (ajouter, modifier, supprimer,
 *        lister, rechercher), la recherche par reference et les alertes
 *        de stock.
 * UTILISE PAR : main.c (menu), commande.c (lecture/mise a jour du stock),
 *        statistique.c (palmares produits).
 * ============================================================ */
// types.h

#ifndef TYPES_H
#define TYPES_H

/* Tailles maximales */
#define MAX_NOM 50
#define MAX_PRENOM 50
#define MAX_EMAIL 80
#define MAX_TELEPHONE 15

/* Tailles des champs (produits / statut) */
#define LG_REFERENCE   12
#define LG_DESIGNATION 60
#define LG_CATEGORIE   30
#define LG_STATUT      20

/* Statuts possibles d'une commande */
#define STATUT_EN_COURS "EN_COURS"
#define STATUT_EXPEDIEE "EXPEDIEE"
#define STATUT_LIVREE   "LIVREE"
#define STATUT_ANNULEE  "ANNULEE"

/* Constantes métier */
#define TVA 20.0
#define FRAIS_PORT 5.90
#define CAPACITE_INITIALE 4

/* Structure Date */
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

/* Structure Adresse */
typedef struct {
    char rue[80];
    char codePostal[6];
    char ville[50];
    char pays[50];
} Adresse;

/* Structure Produit */
typedef struct {
    char reference[12];
    char designation[60];
    char categorie[30];
    float prixUnitaireHT;
    int quantiteStock;
    int seuilAlerte;
    int actif;
} Produit;

/* Structure LigneCommande */
typedef struct {
    char refProduit[12];
    char designation[60];
    int quantite;
    float prixUnitaireHT;
    float sousTotalHT;
} LigneCommande;

/* Structure Commande */
typedef struct {
    int numero;
    int idClient;

    Date dateCommande;

    char statut[20];

    int nbLignes;
    int capaciteLignes;
    LigneCommande *lignes;

    float montantBrutHT;
    float tauxRemise;
    float montantRemise;
    float montantHT;
    float montantTVA;
    float montantTTC;
    float fraisPort;

    Adresse adresseLivraison;
} Commande;

/* Structure Client */
typedef struct {
    int idClient;

    char nom[MAX_NOM];
    char prenom[MAX_PRENOM];
    char email[MAX_EMAIL];
    char telephone[MAX_TELEPHONE];

    Adresse adresse;

    int pointsFidelite;

    Commande *commandes;
    int nbCommandes;
    int capaciteCommandes;

    int actif;
} Client;

/* Structure Boutique si qqun peut corriger :* */
typedef struct {
    char nom[50];

    Produit *produits;
    int nbProduits;
    int capaciteProduits;

    Client *clients;
    int nbClients;
    int capaciteClients;

    int numeroSuivant;
    int idClientSuivant;
} Boutique;

#endif
