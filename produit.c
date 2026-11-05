/* ============================================================
 * produit.c - Implementation du catalogue produits
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "produit.h"
#include "utils.h"


#define CAPACITE_INITIALE_PRODUITS 4

/* ============================================================
 * FONCTIONS INTERNES
 * ============================================================ */

static int assurerCapaciteProduits(Boutique *b)
{
    if (b->nbProduits < b->capaciteProduits) return 0;

    int nouvelleCapacite =
        (b->capaciteProduits > 0)
            ? b->capaciteProduits * 2
            : CAPACITE_INITIALE_PRODUITS;

    Produit *tmp = realloc(
        b->produits, (size_t)nouvelleCapacite * sizeof(Produit));

    if (tmp == NULL) return -1;

    b->produits = tmp;
    b->capaciteProduits = nouvelleCapacite;
    return 0;
}

/* Un produit est utilise s'il apparait dans une commande non annulee. */
static int produitUtilise(const Boutique *b, const char *reference)
{
    for (int i = 0; i < b->nbClients; ++i) {
        const Client *client = &b->clients[i];

        for (int j = 0; j < client->nbCommandes; ++j) {
            const Commande *commande = &client->commandes[j];

            if (strcmp(commande->statut, STATUT_ANNULEE) == 0)
                continue;

            for (int k = 0; k < commande->nbLignes; ++k) {
                if (strcmp(commande->lignes[k].refProduit, reference) == 0)
                    return 1;
            }
        }
    }
    return 0;
}


static void afficherEnteteProduits(void)
{
    printf("\n%-12s %-24s %-14s %10s %6s %6s %12s %s\n",
           "Reference", "Designation", "Categorie", "Prix HT",
           "Stock", "Seuil", "Valeur stock", "Alerte");
    printf("-----------------------------------------------------------------------------------------------\n");
}

static void afficherLigneProduit(const Produit *p)
{
    double valeurStock = (double)p->prixUnitaireHT * p->quantiteStock;

    printf("%-12s %-24.24s %-14.14s %10.2f %6d %6d %12.2f %s\n",
           p->reference, p->designation, p->categorie,
           p->prixUnitaireHT, p->quantiteStock, p->seuilAlerte,
           valeurStock,
           p->quantiteStock <= p->seuilAlerte ? "<< STOCK BAS" : "");
}

/* ============================================================
 * RECHERCHE
 * ============================================================ */

int indexProduit(const Boutique *b, const char *reference)
{
    if (b == NULL || reference == NULL) return -1;

    for (int i = 0; i < b->nbProduits; ++i) {
        if (strcmp(b->produits[i].reference, reference) == 0)
            return i;
    }
    return -1;
}

Produit *trouverProduitActif(Boutique *b, const char *reference)
{
    int index = indexProduit(b, reference);
    if (index < 0 || !b->produits[index].actif) return NULL;
    return &b->produits[index];
}

/* ============================================================
 * AJOUT
 * ============================================================ */

int ajouterProduitDonnees(Boutique *b, const Produit *p)
{
    if (b == NULL || p == NULL) return PRODUIT_MEMOIRE;
    if (indexProduit(b, p->reference) != -1) return PRODUIT_DOUBLON;
    if (assurerCapaciteProduits(b) != 0) return PRODUIT_MEMOIRE;

    b->produits[b->nbProduits++] = *p;
    return PRODUIT_OK;
}

void ajouterProduit(Boutique *b)
{
    if (b == NULL) return;

    Produit p;
    memset(&p, 0, sizeof p);

    printf("\n--- Ajout d'un produit ---\n");

    for (;;) {
        lireChaine("Reference (ex. PRD-0009) : ",
                   p.reference, sizeof p.reference);

        if (indexProduit(b, p.reference) == -1) break;
        printf("Erreur : cette reference existe deja (meme desactivee).\n");
    }

    lireChaine("Designation : ", p.designation, sizeof p.designation);
    lireChaine("Categorie   : ", p.categorie, sizeof p.categorie);

    p.prixUnitaireHT = (float)lireReel("Prix unitaire HT (> 0) : ", 0.0, 1);
    p.quantiteStock = lireEntier("Stock initial (>= 0) : ", 0, STOCK_MAX);
    p.seuilAlerte = lireEntier("Seuil d'alerte (>= 0) : ", 0, STOCK_MAX);
    p.actif = 1;

    switch (ajouterProduitDonnees(b, &p)) {
        case PRODUIT_OK:
            printf("Produit %s ajoute.\n", p.reference);
            break;
        case PRODUIT_DOUBLON:
            printf("Erreur : reference deja utilisee.\n");
            break;
        default:
            printf("Erreur : memoire insuffisante, produit non ajoute.\n");
            break;
    }
}

/* ============================================================
 * MODIFICATION
 * ============================================================ */

void modifierProduit(Boutique *b)
{
    if (b == NULL) return;

    char reference[LG_REFERENCE];

    printf("\n--- Modification d'un produit ---\n");
    lireChaine("Reference du produit : ", reference, sizeof reference);

    Produit *p = trouverProduitActif(b, reference);
    if (p == NULL) {
        printf("Erreur : aucun produit actif avec la reference %s.\n",
               reference);
        return;
    }

    printf("%s - %s | prix : %.2f | stock : %d | seuil : %d\n",
           p->reference, p->designation, p->prixUnitaireHT,
           p->quantiteStock, p->seuilAlerte);

    p->prixUnitaireHT =
        (float)lireReel("Nouveau prix HT (> 0) : ", 0.0, 1);
    p->quantiteStock =
        lireEntier("Nouveau stock (>= 0) : ", 0, STOCK_MAX);
    p->seuilAlerte =
        lireEntier("Nouveau seuil d'alerte (>= 0) : ", 0, STOCK_MAX);

    printf("Produit %s mis a jour.\n", p->reference);
}

/* Ancien nom conserve pour compatibilite. */
void mettreAJourProduit(Boutique *b)
{
    modifierProduit(b);
}

/* ============================================================
 * SUPPRESSION LOGIQUE
 * ============================================================ */

void supprimerProduit(Boutique *b)
{
    if (b == NULL) return;

    char reference[LG_REFERENCE];

    printf("\n--- Suppression d'un produit ---\n");
    lireChaine("Reference du produit : ", reference, sizeof reference);

    Produit *p = trouverProduitActif(b, reference);
    if (p == NULL) {
        printf("Erreur : aucun produit actif avec la reference %s.\n",
               reference);
        return;
    }

    if (produitUtilise(b, reference)) {
        printf("Suppression refusee : %s figure dans une commande non annulee.\n",
               reference);
        return;
    }

    p->actif = 0;
    printf("Produit %s supprime (logiquement).\n", reference);
}

/* ============================================================
 * AFFICHAGE ET RECHERCHE
 * ============================================================ */

void afficherCatalogue(const Boutique *b)
{
    if (b == NULL) return;

    double total = 0.0;
    int nombre = 0;

    printf("\n=== CATALOGUE DES PRODUITS ===");
    afficherEnteteProduits();

    for (int i = 0; i < b->nbProduits; ++i) {
        const Produit *p = &b->produits[i];
        if (!p->actif) continue;

        afficherLigneProduit(p);
        total += (double)p->prixUnitaireHT * p->quantiteStock;
        ++nombre;
    }

    if (nombre == 0) {
        printf("(catalogue vide)\n");
    } else {
        printf("\n%d produit(s) - valorisation totale du stock : %.2f EUR HT\n",
               nombre, total);
    }
}

/* Ancien nom conserve pour compatibilite. */
void afficherProduits(const Boutique *b)
{
    afficherCatalogue(b);
}

void rechercherProduit(const Boutique *b)
{
    if (b == NULL) return;

    printf("\n--- Recherche d'un produit ---\n");
    printf("1. Par reference exacte\n");
    printf("2. Par mot-cle dans la designation\n");

    int choix = lireEntier("Votre choix : ", 1, 2);
    int trouves = 0;

    if (choix == 1) {
        char reference[LG_REFERENCE];
        lireChaine("Reference : ", reference, sizeof reference);

        int index = indexProduit(b, reference);
        if (index >= 0 && b->produits[index].actif) {
            afficherEnteteProduits();
            afficherLigneProduit(&b->produits[index]);
            trouves = 1;
        } else if (index >= 0) {
            printf("Ce produit existe mais a ete supprime (desactive).\n");
            return;
        }
    } else {
        char mot[LG_DESIGNATION];
        lireChaine("Mot-cle : ", mot, sizeof mot);

        for (int i = 0; i < b->nbProduits; ++i) {
            const Produit *p = &b->produits[i];

            if (!p->actif || !contientMotCle(p->designation, mot))
                continue;

            if (trouves == 0) afficherEnteteProduits();
            afficherLigneProduit(p);
            ++trouves;
        }
    }

    if (trouves == 0) printf("Aucun produit trouve.\n");
}

void afficherAlertesStock(const Boutique *b)
{
    if (b == NULL) return;

    int nombre = 0;
    printf("\n=== ALERTES DE STOCK ===");

    for (int i = 0; i < b->nbProduits; ++i) {
        const Produit *p = &b->produits[i];

        if (!p->actif || p->quantiteStock > p->seuilAlerte)
            continue;

        if (nombre == 0) afficherEnteteProduits();
        afficherLigneProduit(p);
        ++nombre;
    }

    if (nombre == 0) {
        printf("\nAucun produit en alerte de stock.\n");
    } else {
        printf("\n%d produit(s) a reapprovisionner.\n", nombre);
    }
}

/* ============================================================
 * GESTION DU STOCK
 * ============================================================ */

int retirerStock(Produit *p, int quantite)
{
    if (p == NULL || quantite <= 0 || quantite > p->quantiteStock)
        return PRODUIT_STOCK_INSUFFISANT;

    p->quantiteStock -= quantite;
    return PRODUIT_OK;
}

void reintegrerStock(Produit *p, int quantite)
{
    if (p == NULL || quantite <= 0) return;

    if (p->quantiteStock <= STOCK_MAX - quantite)
        p->quantiteStock += quantite;
}
