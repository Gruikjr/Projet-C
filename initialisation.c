/* ============================================================
 * initialisation.c - Donnees de demonstration + liberation memoire
 * ------------------------------------------------------------
 * ROLE : - initialiserBoutique : met la Boutique a zero (compteurs
 *          numeroSuivant = 1001 et idClientSuivant = 1)
 *        - initialiserDonneesTest : 8 produits (4 categories, 2 en alerte),
 *          5 clients, 6 commandes (4 statuts, 4 commandes de 3 lignes ou
 *          plus dont une > 600 EUR HT)
 *        - detruireBoutique : free() de TOUS les niveaux.
 * NB : les commandes sont construites avec les vraies fonctions metier
 *      (creerCommande, ajouterLigne...), donc stock, montants et points
 *      de fidelite sont coherents. Les stocks de depart sont majores des
 *      quantites vendues pour que les stocks FINAUX soient ceux du sujet.
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "initialisation.h"
#include "produit.h"
#include "client.h"
#include "commande.h"

#define CAPACITE_PRODUITS_INIT 10
#define CAPACITE_CLIENTS_INIT   8

int initialiserBoutique(Boutique *b)
{
    if (b == NULL) return 0;

    memset(b, 0, sizeof *b);
    strcpy(b->nom, "ShopFlow");
    b->numeroSuivant   = 1001;
    b->idClientSuivant = 1;

    b->produits = malloc(CAPACITE_PRODUITS_INIT * sizeof(Produit));
    if (b->produits == NULL) return 0;
    b->capaciteProduits = CAPACITE_PRODUITS_INIT;

    b->clients = malloc(CAPACITE_CLIENTS_INIT * sizeof(Client));
    if (b->clients == NULL) {
        free(b->produits);
        b->produits = NULL;
        b->capaciteProduits = 0;
        return 0;
    }
    b->capaciteClients = CAPACITE_CLIENTS_INIT;
    return 1;
}

/* Cree une commande complete a partir d'un tableau de lignes, puis la
 * fait avancer jusqu'au statut demande. Retourne 1 si tout s'est bien passe. */
typedef struct { const char *ref; int quantite; } LigneTest;

static int commandeTest(Boutique *b, int idClient, Date date,
                        const LigneTest *lignes, int nbLignes,
                        const char *statutFinal)
{
    int numero = creerCommande(b, idClient, date);
    if (numero <= 0) return 0;

    for (int i = 0; i < nbLignes; ++i)
        if (ajouterLigne(b, idClient, numero, lignes[i].ref,
                         lignes[i].quantite) != CMD_OK)
            return 0;

    if (strcmp(statutFinal, STATUT_ANNULEE) == 0)
        return annulerCommande(b, idClient, numero) == CMD_OK;

    if (strcmp(statutFinal, STATUT_EXPEDIEE) == 0 ||
        strcmp(statutFinal, STATUT_LIVREE) == 0)
        if (changerStatutCommande(b, idClient, numero, STATUT_EXPEDIEE) != CMD_OK)
            return 0;

    if (strcmp(statutFinal, STATUT_LIVREE) == 0)
        return changerStatutCommande(b, idClient, numero, STATUT_LIVREE) == CMD_OK;

    return 1;
}

int initialiserDonneesTest(Boutique *b)
{
    if (b == NULL) return 0;

    /* ---------- 1. Catalogue : 8 produits, 4 categories ---------- */
    /* PRD-0002 (8 <= 15) et PRD-0007 (4 <= 10) finissent en alerte. */
    const Produit produits[] = {
        {"PRD-0001", "Clavier mecanique",   "Informatique",  79.90f,  45, 10, 1},
        {"PRD-0002", "Souris sans fil",     "Informatique",  24.50f,  11, 15, 1},
        {"PRD-0003", "Ecran 27 pouces",     "Informatique", 199.00f,   8,  5, 1},
        {"PRD-0004", "Casque audio",        "Audio",         89.90f,  28, 10, 1},
        {"PRD-0005", "Enceinte Bluetooth",  "Audio",         59.90f,  14,  8, 1},
        {"PRD-0006", "Cable HDMI 2 m",      "Accessoires",    9.90f, 128, 30, 1},
        {"PRD-0007", "Hub USB-C 7 ports",   "Accessoires",   49.90f,   6, 10, 1},
        {"PRD-0008", "Disque SSD 1 To",     "Stockage",     109.00f,  20,  6, 1}
    };
    for (size_t i = 0; i < sizeof produits / sizeof produits[0]; ++i)
        if (ajouterProduitDonnees(b, &produits[i]) != PRODUIT_OK) return 0;

    /* ---------- 2. Clients : 5 clients (ids 1 a 5 via le compteur) ---------- */
    const Client clients[] = {
        {0, "Bernard", "Julie",  "j.bernard@mail.fr", "0612345678",
         {"12 rue de la Paix",       "Paris",     "75002", "France"}, 0, NULL, 0, 0, 1},
        {0, "Martin",  "Lucas",  "l.martin@mail.fr",  "0623456789",
         {"8 avenue Victor Hugo",    "Lyon",      "69003", "France"}, 0, NULL, 0, 0, 1},
        {0, "Dubois",  "Emma",   "e.dubois@mail.fr",  "0634567890",
         {"45 boulevard Gambetta",   "Marseille", "13001", "France"}, 0, NULL, 0, 0, 1},
        {0, "Petit",   "Hugo",   "h.petit@mail.fr",   "0645678901",
         {"3 place du Capitole",     "Toulouse",  "31000", "France"}, 0, NULL, 0, 0, 1},
        {0, "Moreau",  "Lea",    "l.moreau@mail.fr",  "0656789012",
         {"27 rue Sainte-Catherine", "Bordeaux",  "33000", "France"}, 0, NULL, 0, 0, 1}
    };
    for (size_t i = 0; i < sizeof clients / sizeof clients[0]; ++i)
        if (!ajouterClient(b, clients[i])) return 0;

    /* ---------- 3. Commandes ---------- */
    /* 1001 : LIVREE, 3 lignes, 279.40 HT brut -> remise 5 % */
    const LigneTest c1[] = {{"PRD-0001", 2}, {"PRD-0006", 3}, {"PRD-0004", 1}};
    /* 1002 : LIVREE, 3 lignes, 695.90 HT brut -> remise 15 % (> 600 EUR) */
    const LigneTest c2[] = {{"PRD-0003", 2}, {"PRD-0008", 2}, {"PRD-0001", 1}};
    /* 1003 : EXPEDIEE, 2 lignes, 84.40 HT brut -> pas de remise, port offert */
    const LigneTest c3[] = {{"PRD-0005", 1}, {"PRD-0002", 1}};
    /* 1004 : EN_COURS, 3 lignes, 198.30 HT brut -> remise 5 % */
    const LigneTest c4[] = {{"PRD-0007", 2}, {"PRD-0006", 5}, {"PRD-0002", 2}};
    /* 1005 : ANNULEE (stock reintegre, points retires), 1 ligne 89.90 */
    const LigneTest c5[] = {{"PRD-0004", 1}};
    /* 1006 : LIVREE, 2 lignes, 239.70 HT brut -> remise 5 % */
    const LigneTest c6[] = {{"PRD-0004", 2}, {"PRD-0005", 1}};

    return commandeTest(b, 1, (Date){12,  3, 2026}, c1, 3, STATUT_LIVREE)   &&
           commandeTest(b, 2, (Date){25,  3, 2026}, c2, 3, STATUT_LIVREE)   &&
           commandeTest(b, 3, (Date){ 2, 10, 2026}, c3, 2, STATUT_EXPEDIEE) &&
           commandeTest(b, 1, (Date){28, 10, 2026}, c4, 3, STATUT_EN_COURS) &&
           commandeTest(b, 4, (Date){15,  9, 2026}, c5, 1, STATUT_ANNULEE)  &&
           commandeTest(b, 5, (Date){18, 10, 2026}, c6, 2, STATUT_LIVREE);
}

void detruireBoutique(Boutique *b)
{
    if (b == NULL) return;

    for (int i = 0; i < b->nbClients; i++) {
        for (int j = 0; j < b->clients[i].nbCommandes; j++)
            free(b->clients[i].commandes[j].lignes);
        free(b->clients[i].commandes);
    }
    free(b->clients);
    free(b->produits);

    b->clients = NULL;
    b->produits = NULL;
    b->nbClients = b->nbProduits = 0;
    b->capaciteClients = b->capaciteProduits = 0;
}
