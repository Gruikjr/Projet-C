/* ============================================================
 * commande.c - Implementation des commandes
 * ------------------------------------------------------------
 * ROLE : gere le tableau dynamique client->commandes et, pour chaque
 *        commande, le tableau dynamique lignes (malloc/realloc/free).
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commande.h"
#include "client.h"
#include "produit.h"
#include "utils.h"

#define CAPACITE_INITIALE_COMMANDES 2
#define CAPACITE_INITIALE_LIGNES    4

/* ============================================================
 * OUTILS INTERNES
 * ============================================================ */

static double arrondi2(double x)
{
    return (double)(long long)(x * 100.0 + 0.5) / 100.0;   /* x >= 0 */
}

/* Points de fidelite lies a une commande : 1 par tranche de 10 EUR TTC */
static int pointsDeCommande(const Commande *c)
{
    return (int)(c->montantTTC / 10.0 + 1e-4);
}

/* Retrouve le client (actif) et la commande correspondants */
static int localiser(Boutique *b, int idClient, int numero,
                     Client **client, Commande **commande)
{
    if (b == NULL) return CMD_ERR_INTROUVABLE;

    int pos = trouverClient(b, idClient);
    if (pos == 0) return CMD_ERR_INTROUVABLE;

    Client *cl = &b->clients[pos - 1];
    for (int i = 0; i < cl->nbCommandes; ++i) {
        if (cl->commandes[i].numero == numero) {
            *client = cl;
            *commande = &cl->commandes[i];
            return CMD_OK;
        }
    }
    return CMD_ERR_INTROUVABLE;
}

/* Recalcule la commande et ajuste les points du client de la difference */
static void recalculerEtAjusterPoints(Boutique *b, Commande *c)
{
    int avant = pointsDeCommande(c);
    calculCommande(c);
    int delta = pointsDeCommande(c) - avant;

    if (delta > 0)      ajouterPoints(b, c->idClient, delta);
    else if (delta < 0) retirerPoints(b, c->idClient, -delta);
}

/* Remet en stock toutes les lignes d'une commande */
static void reintegrerLignes(Boutique *b, const Commande *c)
{
    for (int i = 0; i < c->nbLignes; ++i) {
        int idx = indexProduit(b, c->lignes[i].refProduit);
        if (idx >= 0) reintegrerStock(&b->produits[idx], c->lignes[i].quantite);
    }
}

const char *messageCommande(int code)
{
    switch (code) {
        case CMD_OK:                return "Operation reussie.";
        case CMD_ERR_INTROUVABLE:   return "Client ou commande introuvable.";
        case CMD_ERR_STATUT:        return "Action impossible pour le statut actuel de la commande.";
        case CMD_ERR_PRODUIT:       return "Produit inconnu ou supprime.";
        case CMD_ERR_DOUBLON:       return "Ce produit figure deja dans la commande.";
        case CMD_ERR_STOCK:         return "Stock insuffisant.";
        case CMD_ERR_MEMOIRE:       return "Memoire insuffisante.";
        case CMD_ERR_QUANTITE:      return "Quantite invalide.";
        case CMD_ERR_DERNIERE_LIGNE:return "Derniere ligne : annulez plutot la commande.";
        case CMD_ERR_DATE:          return "Date invalide.";
        default:                    return "Erreur inconnue.";
    }
}

/* ============================================================
 * CALCUL DES MONTANTS
 * ============================================================ */

void calculCommande(Commande *c)
{
    if (c == NULL) return;

    double brut = 0.0;
    for (int i = 0; i < c->nbLignes; ++i) {
        c->lignes[i].sousTotalHT =
            (float)arrondi2((double)c->lignes[i].quantite *
                            c->lignes[i].prixUnitaireHT);
        brut += c->lignes[i].sousTotalHT;
    }
    brut = arrondi2(brut);

    /* Palier de remise determine sur le HT brut */
    double taux = 0.0;
    if (brut >= 600.0)      taux = 15.0;
    else if (brut >= 300.0) taux = 10.0;
    else if (brut >= 100.0) taux = 5.0;

    double remise = arrondi2(brut * taux / 100.0);
    double ht     = arrondi2(brut - remise);
    double tva    = arrondi2(ht * TVA / 100.0);
    double port   = (c->nbLignes > 0 && brut < SEUIL_PORT_OFFERT) ? FRAIS_PORT : 0.0;

    c->montantBrutHT = (float)brut;
    c->tauxRemise    = (float)taux;
    c->montantRemise = (float)remise;
    c->montantHT     = (float)ht;
    c->montantTVA    = (float)tva;
    c->fraisPort     = (float)port;
    c->montantTTC    = (float)arrondi2(ht + tva + port);
}

/* ============================================================
 * CREATION
 * ============================================================ */

int creerCommande(Boutique *b, int idClient, Date date)
{
    if (b == NULL) return CMD_ERR_INTROUVABLE;

    int pos = trouverClient(b, idClient);
    if (pos == 0) return CMD_ERR_INTROUVABLE;
    if (!dateValide(date.jour, date.mois, date.annee)) return CMD_ERR_DATE;

    Client *cl = &b->clients[pos - 1];

    if (cl->nbCommandes >= cl->capaciteCommandes) {
        int nouvelle = (cl->capaciteCommandes > 0)
                     ? cl->capaciteCommandes * 2 : CAPACITE_INITIALE_COMMANDES;
        Commande *tmp = realloc(cl->commandes, (size_t)nouvelle * sizeof(Commande));
        if (tmp == NULL) return CMD_ERR_MEMOIRE;
        cl->commandes = tmp;
        cl->capaciteCommandes = nouvelle;
    }

    LigneCommande *lignes =
        malloc(CAPACITE_INITIALE_LIGNES * sizeof(LigneCommande));
    if (lignes == NULL) return CMD_ERR_MEMOIRE;

    Commande nouvelle;
    memset(&nouvelle, 0, sizeof nouvelle);
    nouvelle.numero         = b->numeroSuivant;
    nouvelle.idClient       = idClient;
    nouvelle.dateCommande   = date;
    strcpy(nouvelle.statut, STATUT_EN_COURS);
    nouvelle.lignes         = lignes;
    nouvelle.capaciteLignes = CAPACITE_INITIALE_LIGNES;
    nouvelle.adresseLivraison = cl->adresse;      /* instantane de l'adresse */

    cl->commandes[cl->nbCommandes++] = nouvelle;
    b->numeroSuivant++;
    return nouvelle.numero;
}

void abandonnerCommandeVide(Boutique *b, int idClient, int numero)
{
    Client *cl;
    Commande *c;
    if (localiser(b, idClient, numero, &cl, &c) != CMD_OK) return;
    if (c->nbLignes != 0) return;

    /* On ne retire que la derniere commande du client (celle qu'on vient de creer) */
    if (c != &cl->commandes[cl->nbCommandes - 1]) return;

    free(c->lignes);
    cl->nbCommandes--;
    if (numero == b->numeroSuivant - 1) b->numeroSuivant--;
}

/* ============================================================
 * LIGNES
 * ============================================================ */

int ajouterLigne(Boutique *b, int idClient, int numero,
                 const char *reference, int quantite)
{
    Client *cl;
    Commande *c;
    int code = localiser(b, idClient, numero, &cl, &c);
    if (code != CMD_OK) return code;

    if (strcmp(c->statut, STATUT_EN_COURS) != 0) return CMD_ERR_STATUT;
    if (quantite <= 0) return CMD_ERR_QUANTITE;

    Produit *p = trouverProduitActif(b, reference);
    if (p == NULL) return CMD_ERR_PRODUIT;

    for (int i = 0; i < c->nbLignes; ++i)
        if (strcmp(c->lignes[i].refProduit, p->reference) == 0)
            return CMD_ERR_DOUBLON;

    if (quantite > p->quantiteStock) return CMD_ERR_STOCK;

    if (c->nbLignes >= c->capaciteLignes) {
        int nouvelle = (c->capaciteLignes > 0)
                     ? c->capaciteLignes * 2 : CAPACITE_INITIALE_LIGNES;
        LigneCommande *tmp =
            realloc(c->lignes, (size_t)nouvelle * sizeof(LigneCommande));
        if (tmp == NULL) return CMD_ERR_MEMOIRE;
        c->lignes = tmp;
        c->capaciteLignes = nouvelle;
    }

    LigneCommande *l = &c->lignes[c->nbLignes];
    memset(l, 0, sizeof *l);
    strcpy(l->refProduit, p->reference);
    strcpy(l->designation, p->designation);
    l->quantite       = quantite;
    l->prixUnitaireHT = p->prixUnitaireHT;      /* prix fige a la vente */

    retirerStock(p, quantite);
    c->nbLignes++;
    recalculerEtAjusterPoints(b, c);
    return CMD_OK;
}

int supprimerLigne(Boutique *b, int idClient, int numero,
                   const char *reference)
{
    Client *cl;
    Commande *c;
    int code = localiser(b, idClient, numero, &cl, &c);
    if (code != CMD_OK) return code;

    if (strcmp(c->statut, STATUT_EN_COURS) != 0) return CMD_ERR_STATUT;

    int pos = -1;
    for (int i = 0; i < c->nbLignes; ++i)
        if (strcmp(c->lignes[i].refProduit, reference) == 0) { pos = i; break; }
    if (pos < 0) return CMD_ERR_PRODUIT;
    if (c->nbLignes == 1) return CMD_ERR_DERNIERE_LIGNE;

    int idx = indexProduit(b, reference);
    if (idx >= 0) reintegrerStock(&b->produits[idx], c->lignes[pos].quantite);

    for (int i = pos; i + 1 < c->nbLignes; ++i)
        c->lignes[i] = c->lignes[i + 1];
    c->nbLignes--;

    recalculerEtAjusterPoints(b, c);
    return CMD_OK;
}

/* ============================================================
 * STATUT
 * ============================================================ */

int changerStatutCommande(Boutique *b, int idClient, int numero,
                          const char *nouveauStatut)
{
    Client *cl;
    Commande *c;
    int code = localiser(b, idClient, numero, &cl, &c);
    if (code != CMD_OK) return code;
    if (nouveauStatut == NULL) return CMD_ERR_STATUT;

    /* Progression stricte : EN_COURS -> EXPEDIEE -> LIVREE */
    if (strcmp(c->statut, STATUT_EN_COURS) == 0 &&
        strcmp(nouveauStatut, STATUT_EXPEDIEE) == 0) {
        strcpy(c->statut, STATUT_EXPEDIEE);
        return CMD_OK;
    }
    if (strcmp(c->statut, STATUT_EXPEDIEE) == 0 &&
        strcmp(nouveauStatut, STATUT_LIVREE) == 0) {
        strcpy(c->statut, STATUT_LIVREE);
        return CMD_OK;
    }
    return CMD_ERR_STATUT;
}

int annulerCommande(Boutique *b, int idClient, int numero)
{
    Client *cl;
    Commande *c;
    int code = localiser(b, idClient, numero, &cl, &c);
    if (code != CMD_OK) return code;

    if (strcmp(c->statut, STATUT_EN_COURS) != 0) return CMD_ERR_STATUT;

    reintegrerLignes(b, c);
    retirerPoints(b, idClient, pointsDeCommande(c));
    strcpy(c->statut, STATUT_ANNULEE);
    return CMD_OK;
}

/* ============================================================
 * RECHERCHE
 * ============================================================ */

Commande *chercherCommande(Boutique *b, int idClient, int numero)
{
    Client *cl;
    Commande *c;
    if (localiser(b, idClient, numero, &cl, &c) != CMD_OK) return NULL;
    return c;
}

Commande *chercherCommandeParNumero(Boutique *b, int numero)
{
    if (b == NULL) return NULL;
    for (int i = 0; i < b->nbClients; ++i)
        for (int j = 0; j < b->clients[i].nbCommandes; ++j)
            if (b->clients[i].commandes[j].numero == numero)
                return &b->clients[i].commandes[j];
    return NULL;
}

/* ============================================================
 * AFFICHAGE
 * ============================================================ */

void afficherCommande(const Commande *c)
{
    if (c == NULL) return;

    printf("\n========== COMMANDE %d ==========\n", c->numero);
    printf("Client : %d | Date : %02d/%02d/%04d | Statut : %s\n",
           c->idClient, c->dateCommande.jour, c->dateCommande.mois,
           c->dateCommande.annee, c->statut);
    printf("Livraison : %s, %s %s, %s\n", c->adresseLivraison.rue,
           c->adresseLivraison.codePostal, c->adresseLivraison.ville,
           c->adresseLivraison.pays);

    printf("\n%-12s %-24s %5s %10s %11s\n",
           "Reference", "Designation", "Qte", "PU HT", "Sous-total");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < c->nbLignes; ++i) {
        const LigneCommande *l = &c->lignes[i];
        printf("%-12s %-24.24s %5d %10.2f %11.2f\n", l->refProduit,
               l->designation, l->quantite, l->prixUnitaireHT, l->sousTotalHT);
    }
    printf("--------------------------------------------------------------------\n");
    printf("Montant HT brut        : %10.2f EUR\n", c->montantBrutHT);
    printf("Remise (%4.1f %%)        : %10.2f EUR\n", c->tauxRemise, c->montantRemise);
    printf("Montant HT             : %10.2f EUR\n", c->montantHT);
    printf("TVA (%.0f %%)             : %10.2f EUR\n", TVA, c->montantTVA);
    printf("Frais de port          : %10.2f EUR%s\n", c->fraisPort,
           c->fraisPort == 0.0f && c->nbLignes > 0 ? "  (offerts)" : "");
    printf("TOTAL TTC              : %10.2f EUR\n", c->montantTTC);
}

void afficherCommandes(Boutique *b, int idClient)
{
    int pos = (b != NULL) ? trouverClient(b, idClient) : 0;
    if (pos == 0) {
        printf("Client introuvable.\n");
        return;
    }

    const Client *cl = &b->clients[pos - 1];
    if (cl->nbCommandes == 0) {
        printf("Ce client n'a aucune commande.\n");
        return;
    }
    for (int i = 0; i < cl->nbCommandes; ++i)
        afficherCommande(&cl->commandes[i]);
}
