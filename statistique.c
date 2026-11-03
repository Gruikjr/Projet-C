/* ============================================================
 * statistique.c - Implementation des statistiques
 * ------------------------------------------------------------
 * ROLE : parcourt clients -> commandes -> lignes pour agreger les donnees.
 *        Les palmares utilisent un tableau temporaire (calloc) trie avec
 *        qsort, puis libere (free).
 * REGLE : les commandes au statut ANNULEE sont exclues partout.
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statistique.h"
#include "produit.h"   /* indexProduit */
#include "utils.h"     /* lireEntier   */

/* Une ligne du palmares produits */
typedef struct {
    char reference[LG_REFERENCE];
    char designation[LG_DESIGNATION];
    int  quantite;      /* quantite totale vendue */
    double caHT;        /* CA HT brut des lignes   */
} StatProduit;

/* Une ligne du palmares clients */
typedef struct {
    int  idClient;
    char nom[MAX_NOM];
    char prenom[MAX_PRENOM];
    int  nbCommandes;   /* commandes non annulees */
    double caHT;
    double caTTC;
} StatClient;

/* ============================================================
 * OUTILS INTERNES
 * ============================================================ */

static int estAnnulee(const Commande *c)
{
    return strcmp(c->statut, STATUT_ANNULEE) == 0;
}

/* Comparateurs qsort : ordre decroissant, egalites departagees pour un
 * classement stable et reproductible. */
static int comparerProduits(const void *pa, const void *pb)
{
    const StatProduit *a = pa, *b = pb;
    if (a->quantite != b->quantite) return b->quantite - a->quantite;
    return strcmp(a->reference, b->reference);
}

static int comparerClients(const void *pa, const void *pb)
{
    const StatClient *a = pa, *b = pb;
    if (a->caHT > b->caHT) return -1;
    if (a->caHT < b->caHT) return 1;
    return a->idClient - b->idClient;
}

/* ============================================================
 * CHIFFRE D'AFFAIRES
 * ============================================================ */

double calculerChiffreAffaires(const Boutique *b, int mois, int annee,
                               double *ttc)
{
    double totalHT = 0.0, totalTTC = 0.0;

    if (b != NULL) {
        for (int i = 0; i < b->nbClients; ++i) {
            const Client *cl = &b->clients[i];
            for (int j = 0; j < cl->nbCommandes; ++j) {
                const Commande *c = &cl->commandes[j];
                if (estAnnulee(c)) continue;
                if (annee != 0 && c->dateCommande.annee != annee) continue;
                if (mois  != 0 && c->dateCommande.mois  != mois)  continue;
                totalHT  += c->montantHT;
                totalTTC += c->montantTTC;
            }
        }
    }
    if (ttc != NULL) *ttc = totalTTC;
    return totalHT;
}

void afficherChiffreAffaires(const Boutique *b)
{
    if (b == NULL) return;

    double ttc;
    double ht = calculerChiffreAffaires(b, 0, 0, &ttc);

    printf("\n=== CHIFFRE D'AFFAIRES (commandes annulees exclues) ===\n");
    printf("Total boutique : %10.2f EUR HT | %10.2f EUR TTC\n", ht, ttc);

    printf("\nConsulter une periode ?\n");
    printf("  1. Une annee\n  2. Un mois\n  0. Non\n");
    int choix = lireEntier("Votre choix : ", 0, 2);
    if (choix == 0) return;

    int annee = lireEntier("Annee (2000-2100) : ", 2000, 2100);
    int mois = 0;
    if (choix == 2) mois = lireEntier("Mois (1-12) : ", 1, 12);

    ht = calculerChiffreAffaires(b, mois, annee, &ttc);
    if (mois != 0)
        printf("\nCA %02d/%d : %10.2f EUR HT | %10.2f EUR TTC\n",
               mois, annee, ht, ttc);
    else
        printf("\nCA %d : %10.2f EUR HT | %10.2f EUR TTC\n", annee, ht, ttc);
}

/* ============================================================
 * PALMARES PRODUITS (top 5 par quantite vendue)
 * ============================================================ */

void afficherPalmaresProduits(const Boutique *b)
{
    if (b == NULL) return;

    printf("\n=== PALMARES PRODUITS (top %d par quantite vendue) ===\n",
           TAILLE_PALMARES_PRODUITS);

    if (b->nbProduits == 0) {
        printf("Aucun produit au catalogue.\n");
        return;
    }

    /* Un emplacement par produit du catalogue : pas de realloc necessaire */
    StatProduit *stats = calloc((size_t)b->nbProduits, sizeof *stats);
    if (stats == NULL) {
        printf("Erreur : memoire insuffisante.\n");
        return;
    }

    for (int i = 0; i < b->nbClients; ++i) {
        const Client *cl = &b->clients[i];
        for (int j = 0; j < cl->nbCommandes; ++j) {
            const Commande *c = &cl->commandes[j];
            if (estAnnulee(c)) continue;

            for (int k = 0; k < c->nbLignes; ++k) {
                const LigneCommande *l = &c->lignes[k];
                int idx = indexProduit(b, l->refProduit);
                if (idx < 0) continue;      /* reference inconnue : ignoree */

                StatProduit *s = &stats[idx];
                if (s->quantite == 0) {     /* premiere vente : on fige les noms */
                    strcpy(s->reference, l->refProduit);
                    strcpy(s->designation, l->designation);
                }
                s->quantite += l->quantite;
                s->caHT     += l->sousTotalHT;
            }
        }
    }

    qsort(stats, (size_t)b->nbProduits, sizeof *stats, comparerProduits);

    printf("\n%-5s %-12s %-26s %8s %12s\n",
           "Rang", "Reference", "Designation", "Qte", "CA HT brut");
    printf("-------------------------------------------------------------------\n");

    int rang = 0;
    for (int i = 0; i < b->nbProduits && rang < TAILLE_PALMARES_PRODUITS; ++i) {
        if (stats[i].quantite == 0) break;  /* le reste n'a rien vendu */
        ++rang;
        printf("%-5d %-12s %-26.26s %8d %12.2f\n", rang, stats[i].reference,
               stats[i].designation, stats[i].quantite, stats[i].caHT);
    }
    if (rang == 0) printf("Aucune vente enregistree.\n");

    free(stats);
}

/* ============================================================
 * PALMARES CLIENTS (par chiffre d'affaires genere)
 * ============================================================ */

void afficherPalmaresClients(const Boutique *b)
{
    if (b == NULL) return;

    printf("\n=== PALMARES CLIENTS (par chiffre d'affaires) ===\n");

    if (b->nbClients == 0) {
        printf("Aucun client enregistre.\n");
        return;
    }

    StatClient *stats = calloc((size_t)b->nbClients, sizeof *stats);
    if (stats == NULL) {
        printf("Erreur : memoire insuffisante.\n");
        return;
    }

    for (int i = 0; i < b->nbClients; ++i) {
        const Client *cl = &b->clients[i];
        StatClient *s = &stats[i];

        s->idClient = cl->idClient;
        strcpy(s->nom, cl->nom);
        strcpy(s->prenom, cl->prenom);

        for (int j = 0; j < cl->nbCommandes; ++j) {
            const Commande *c = &cl->commandes[j];
            if (estAnnulee(c)) continue;
            s->nbCommandes++;
            s->caHT  += c->montantHT;
            s->caTTC += c->montantTTC;
        }
    }

    qsort(stats, (size_t)b->nbClients, sizeof *stats, comparerClients);

    printf("\n%-5s %-4s %-26s %6s %12s %12s\n",
           "Rang", "Id", "Client", "Cmd", "CA HT", "CA TTC");
    printf("-----------------------------------------------------------------------\n");

    int rang = 0;
    for (int i = 0; i < b->nbClients; ++i) {
        if (stats[i].nbCommandes == 0) break;   /* aucun CA : non classe */
        char nomComplet[MAX_NOM + MAX_PRENOM + 2];
        snprintf(nomComplet, sizeof nomComplet, "%s %s",
                 stats[i].prenom, stats[i].nom);
        printf("%-5d %-4d %-26.26s %6d %12.2f %12.2f\n", ++rang,
               stats[i].idClient, nomComplet, stats[i].nbCommandes,
               stats[i].caHT, stats[i].caTTC);
    }
    if (rang == 0) printf("Aucune vente enregistree.\n");

    free(stats);
}