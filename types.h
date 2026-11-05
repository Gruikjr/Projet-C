/* ============================================================
 * types.h - Structures partagees et constantes du projet ShopFlow
 * ------------------------------------------------------------
 * ROLE : declare les 7 structures (Date, Adresse, Produit,
 *        LigneCommande, Commande, Client, Boutique) et les constantes.
 *        Inclus par tous les modules (evite les inclusions circulaires
 *        entre client.h et commande.h).
 * ============================================================ */
#ifndef TYPES_H
#define TYPES_H

/* Tailles des champs (taille du tableau, '\0' compris) */
#define MAX_NOM        50
#define MAX_PRENOM     50
#define MAX_EMAIL      80
#define MAX_TELEPHONE  15
#define MAX_RUE        80
#define MAX_VILLE      50
#define MAX_CODEPOSTAL 6
#define MAX_PAYS       50

#define LG_REFERENCE   12
#define LG_DESIGNATION 60
#define LG_CATEGORIE   30
#define LG_STATUT      20

/* Statuts possibles d'une commande */
#define STATUT_EN_COURS "EN_COURS"
#define STATUT_EXPEDIEE "EXPEDIEE"
#define STATUT_LIVREE   "LIVREE"
#define STATUT_ANNULEE  "ANNULEE"

/* Constantes metier */
#define TVA        20.0     /* % applique sur le HT apres remise        */
#define FRAIS_PORT 5.90     /* EUR, offerts des SEUIL_PORT_OFFERT        */
#define SEUIL_PORT_OFFERT 60.0
#define STOCK_MAX  1000000

/* Structure Date */
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

/* Structure Adresse (ordre impose par le sujet) */
typedef struct {
    char rue[MAX_RUE];
    char ville[MAX_VILLE];
    char codePostal[MAX_CODEPOSTAL];
    char pays[MAX_PAYS];
} Adresse;

/* Structure Produit */
typedef struct {
    char  reference[LG_REFERENCE];
    char  designation[LG_DESIGNATION];
    char  categorie[LG_CATEGORIE];
    float prixUnitaireHT;
    int   quantiteStock;
    int   seuilAlerte;
    int   actif;
} Produit;

/* Structure LigneCommande */
typedef struct {
    char  refProduit[LG_REFERENCE];
    char  designation[LG_DESIGNATION];
    int   quantite;
    float prixUnitaireHT;
    float sousTotalHT;
} LigneCommande;

/* Structure Commande */
typedef struct {
    int  numero;
    int  idClient;
    Date dateCommande;
    char statut[LG_STATUT];

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
    int  idClient;
    char nom[MAX_NOM];
    char prenom[MAX_PRENOM];
    char email[MAX_EMAIL];
    char telephone[MAX_TELEPHONE];
    Adresse adresse;
    int  pointsFidelite;

    Commande *commandes;
    int nbCommandes;
    int capaciteCommandes;

    int actif;
} Client;

/* Structure Boutique (racine) */
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
