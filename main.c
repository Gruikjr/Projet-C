/* ==========================================================================
 * main.c - Point d'entree et menus de ShopFlow
 * --------------------------------------------------------------------------
 * ROLE : cree la Boutique (initialisation.c), charge les donnees de
 *        demonstration, gere les 4 sous-menus puis libere toute la memoire.
 *        Toutes les saisies passent par utils.c (lireEntier, lireChaine...).
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "utils.h"
#include "initialisation.h"
#include "produit.h"
#include "client.h"
#include "commande.h"
#include "statistique.h"

#define ID_MAX 999999

/* --------------------------------------------------------------------------
 * MENU PRODUITS
 * -------------------------------------------------------------------------- */
static void menuProduits(Boutique *b)
{
    int choix;
    do {
        printf("\n===== GESTION DES PRODUITS =====\n");
        printf("  1. Ajouter un produit\n");
        printf("  2. Modifier un produit\n");
        printf("  3. Supprimer un produit\n");
        printf("  4. Afficher le catalogue\n");
        printf("  5. Rechercher un produit\n");
        printf("  0. Retour au menu principal\n");
        choix = lireEntier("  Votre choix : ", 0, 5);

        switch (choix) {
            case 1: ajouterProduit(b);     break;
            case 2: modifierProduit(b);    break;
            case 3: supprimerProduit(b);   break;
            case 4: afficherCatalogue(b);  break;
            case 5: rechercherProduit(b);  break;
            default: break;
        }
    } while (choix != 0);
}

/* --------------------------------------------------------------------------
 * MENU CLIENTS
 * -------------------------------------------------------------------------- */
static void menuClients(Boutique *b)
{
    int choix, id;
    do {
        printf("\n===== GESTION DES CLIENTS =====\n");
        printf("  1. Ajouter un client\n");
        printf("  2. Modifier un client\n");
        printf("  3. Supprimer un client\n");
        printf("  4. Afficher la liste des clients\n");
        printf("  5. Afficher la fiche d'un client\n");
        printf("  0. Retour au menu principal\n");
        choix = lireEntier("  Votre choix : ", 0, 5);

        switch (choix) {
        case 1:
            if (ajouterClient(b, saisirClient()))
                printf("  Client ajoute (ID %d).\n", b->idClientSuivant - 1);
            else
                printf("  [!] Echec : email deja utilise ou memoire insuffisante.\n");
            break;

        case 2:
            id = lireEntier("  ID du client a modifier : ", 1, ID_MAX);
            if (trouverClient(b, id) == 0)
                printf("  [!] Client introuvable.\n");
            else if (modifierClient(b, id, saisirClient()))
                printf("  Client modifie.\n");
            else
                printf("  [!] Echec : email deja utilise par un autre client.\n");
            break;

        case 3:
            id = lireEntier("  ID du client a supprimer : ", 1, ID_MAX);
            if (supprimerClient(b, id))
                printf("  Client supprime.\n");
            else
                printf("  [!] Impossible : client introuvable, deja supprime, ou commandes non annulees.\n");
            break;

        case 4: afficherListeClients(b); break;

        case 5:
            id = lireEntier("  ID du client : ", 1, ID_MAX);
            afficherClient(b, id);
            break;

        default: break;
        }
    } while (choix != 0);
}

/* --------------------------------------------------------------------------
 * MENU COMMANDES
 * -------------------------------------------------------------------------- */

/* Cree une commande : client, date, puis N lignes (au moins une). */
static void menuCreerCommande(Boutique *b)
{
    printf("\n--- Creation d'une commande ---\n");
    int idClient = lireEntier("  ID du client : ", 1, ID_MAX);
    if (trouverClient(b, idClient) == 0) {
        printf("  [!] Client introuvable.\n");
        return;
    }

    Date date = lireDate("  Date de la commande :");
    int numero = creerCommande(b, idClient, date);
    if (numero <= 0) {
        printf("  [!] %s\n", messageCommande(numero));
        return;
    }
    printf("  Commande %d creee. Ajout des lignes :\n", numero);

    int nbAjoutees = 0, encore = 1;
    while (encore) {
        char ref[LG_REFERENCE];
        lireChaine("  Reference produit : ", ref, sizeof ref);
        int qte = lireEntier("  Quantite : ", 1, STOCK_MAX);

        int code = ajouterLigne(b, idClient, numero, ref, qte);
        if (code == CMD_OK) {
            nbAjoutees++;
            printf("  Ligne ajoutee.\n");
        } else {
            printf("  [!] %s\n", messageCommande(code));
        }

        if (nbAjoutees == 0)
            encore = lireEntier("  Reessayer ? (1 = oui, 0 = abandonner) : ", 0, 1);
        else
            encore = lireEntier("  Ajouter une autre ligne ? (1 = oui, 0 = non) : ", 0, 1);
    }

    if (nbAjoutees == 0) {
        abandonnerCommandeVide(b, idClient, numero);
        printf("  Commande abandonnee (aucune ligne).\n");
        return;
    }
    afficherCommande(chercherCommande(b, idClient, numero));
}

/* Demande un numero de commande et retrouve la commande (NULL si absente) */
static Commande *demanderCommande(Boutique *b)
{
    int numero = lireEntier("  Numero de la commande : ", 1, ID_MAX);
    Commande *c = chercherCommandeParNumero(b, numero);
    if (c == NULL) printf("  [!] Commande introuvable.\n");
    return c;
}

static void menuCommandes(Boutique *b)
{
    int choix;
    do {
        printf("\n===== GESTION DES COMMANDES =====\n");
        printf("  1. Creer une commande\n");
        printf("  2. Ajouter une ligne a une commande\n");
        printf("  3. Supprimer une ligne d'une commande\n");
        printf("  4. Changer le statut d'une commande\n");
        printf("  5. Annuler une commande\n");
        printf("  6. Afficher une commande\n");
        printf("  7. Afficher les commandes d'un client\n");
        printf("  0. Retour au menu principal\n");
        choix = lireEntier("  Votre choix : ", 0, 7);

        Commande *c = NULL;
        char ref[LG_REFERENCE];
        int code;

        switch (choix) {
        case 1: menuCreerCommande(b); break;

        case 2:
            if ((c = demanderCommande(b)) == NULL) break;
            lireChaine("  Reference produit : ", ref, sizeof ref);
            code = ajouterLigne(b, c->idClient, c->numero, ref,
                                lireEntier("  Quantite : ", 1, STOCK_MAX));
            printf("  %s\n", code == CMD_OK ? "Ligne ajoutee." : messageCommande(code));
            break;

        case 3:
            if ((c = demanderCommande(b)) == NULL) break;
            lireChaine("  Reference de la ligne a supprimer : ", ref, sizeof ref);
            code = supprimerLigne(b, c->idClient, c->numero, ref);
            printf("  %s\n", code == CMD_OK ? "Ligne supprimee (stock reintegre)." : messageCommande(code));
            break;

        case 4:
            if ((c = demanderCommande(b)) == NULL) break;
            printf("  Statut actuel : %s\n", c->statut);
            printf("  1. EXPEDIEE\n  2. LIVREE\n");
            code = changerStatutCommande(b, c->idClient, c->numero,
                       lireEntier("  Nouveau statut : ", 1, 2) == 1
                           ? STATUT_EXPEDIEE : STATUT_LIVREE);
            printf("  %s\n", code == CMD_OK ? "Statut modifie." : messageCommande(code));
            break;

        case 5:
            if ((c = demanderCommande(b)) == NULL) break;
            code = annulerCommande(b, c->idClient, c->numero);
            printf("  %s\n", code == CMD_OK
                   ? "Commande annulee (stock reintegre, points retires)."
                   : messageCommande(code));
            break;

        case 6:
            if ((c = demanderCommande(b)) != NULL) afficherCommande(c);
            break;

        case 7:
            afficherCommandes(b, lireEntier("  ID du client : ", 1, ID_MAX));
            break;

        default: break;
        }
    } while (choix != 0);
}

/* --------------------------------------------------------------------------
 * MENU STATISTIQUES
 * -------------------------------------------------------------------------- */
static void menuStatistiques(Boutique *b)
{
    int choix;
    do {
        printf("\n===== STATISTIQUES =====\n");
        printf("  1. Chiffre d'affaires (total / mois / annee)\n");
        printf("  2. Palmares produits (top 5)\n");
        printf("  3. Palmares clients\n");
        printf("  4. Alertes de stock\n");
        printf("  5. Statistiques par categorie\n");
        printf("  6. Exporter les commandes en CSV\n");
        printf("  0. Retour au menu principal\n");
        choix = lireEntier("  Votre choix : ", 0, 6);

        switch (choix) {
        case 1: afficherChiffreAffaires(b);  break;
        case 2: afficherPalmaresProduits(b); break;
        case 3: afficherPalmaresClients(b);  break;
        case 4: afficherAlertesStock(b);     break;
        case 5: afficherStatsCategories(b);  break;
        case 6:
            if (exporterCommandesCSV(b, "commandes.csv"))
                printf("  Export ecrit dans commandes.csv\n");
            else
                printf("  [!] Impossible d'ecrire commandes.csv\n");
            break;
        default: break;
        }
    } while (choix != 0);
}

/* --------------------------------------------------------------------------
 * PROGRAMME PRINCIPAL
 * -------------------------------------------------------------------------- */
int main(void)
{
    Boutique boutique;

    if (!initialiserBoutique(&boutique) || !initialiserDonneesTest(&boutique)) {
        fprintf(stderr, "Erreur : initialisation de la boutique impossible.\n");
        detruireBoutique(&boutique);
        return EXIT_FAILURE;
    }

    printf("==============================================\n");
    printf("  SHOPFLOW - Boutique en ligne\n");
    printf("==============================================\n");

    int choix;
    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("  1. Gestion des produits\n");
        printf("  2. Gestion des clients\n");
        printf("  3. Gestion des commandes\n");
        printf("  4. Statistiques\n");
        printf("  0. Quitter\n");
        choix = lireEntier("  Votre choix : ", 0, 4);

        switch (choix) {
        case 1: menuProduits(&boutique);     break;
        case 2: menuClients(&boutique);      break;
        case 3: menuCommandes(&boutique);    break;
        case 4: menuStatistiques(&boutique); break;
        default: printf("\n  Au revoir !\n"); break;
        }
    } while (choix != 0);

    detruireBoutique(&boutique);
    return EXIT_SUCCESS;
}
