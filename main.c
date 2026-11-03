/* ==========================================================================
 * menu.c - Squelette de demarrage pour tester le projet (XTI101)
 * --------------------------------------------------------------------------
 * BUT : un programme qui COMPIL ET TOURNE en 1 commande, des le premier jour.
 *
 *
 * CONTENU : navigation complete (4 sous-menus) + bouchons [STUB] pour
 * chaque fonctionnalite. Vous devez changer les stubs,
 * un par un, par leurs vraies fonctions ecrites dans leurs modules :
 *
 * Le cycle de travail (à répéter pour chaque fonctionnalité)
 * Copy1. CHOISIR une fonctionnalité du menu           (ex. « ajouter un produit »)
 * 2. ÉCRIRE la fonction dans son module           (produit.c)
 * 3. DÉCLARER le prototype dans son header        (produit.h)
 * 4. INCLURE le header dans menu_rapide.c         (#include "produit.h")
 * 5. SUPPRIMER le stub correspondant              (la ligne [STUB] ajouterProduit)
 * 6. RECOMPILER et tester
 *
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "produit.h"
#include "statistique.h"
#include "commande.h"
#include <string.h>
/* --------------------------------------------------------------------------
 * Lecture d'un choix au clavier (version simple, suffisant pour demarrer).
 * A remplacer plus tard par une saisie securisee (module utils.c).
 * -------------------------------------------------------------------------- */
static int lireChoix(void)
{
    int choix;
    printf("  Votre choix : ");
    if (scanf("%d", &choix) != 1)
    {
        exit(EXIT_FAILURE); /* entree fermee : on quitte proprement */
    }
    return choix;
}

/* ==========================================================================
 * STUBS : affichent un message pour PROUVER que l'appel arrive.
 * A REMPLACER par les vraies fonctions des modules au fil des seances.
 * ========================================================================== */

/* --- Module CLIENTS ------------------------------------------------------- */
//void afficherListeClients(void){ printf("  [STUB] afficherListeClients - a implementer dans client.c\n");}

/* ==========================================================================
 * SOUS-MENUS : un seul motif a retenir, applique 4 fois.
 * Pour ajouter une fonctionnalite : un printf + un case + la borne max.
 * ========================================================================== */

void menuProduits(Boutique *b) {
    int choix;

    do {
        printf("\n===== MENU PRODUITS =====\n");
        printf("1. Ajouter un produit\n");
        printf("2. Modifier un produit\n");
        printf("3. Supprimer un produit\n");
        printf("4. Afficher le catalogue\n");
        printf("0. Retour\n");
        printf("Votre choix : ");

        scanf("%d", &choix);

        switch (choix) {
        case 1:
            ajouterProduit(b);
            break;

        case 2:
            modifierProduit(b);
            break;

        case 3:
            supprimerProduit(b);
            break;

        case 4:
            afficherCatalogue(b);
            break;

        case 0:
            printf("Retour au menu principal.\n");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix != 0);
}

static void menuClients(void)
{
    int choix;
    do
    {
        printf("\n===== GESTION DES CLIENTS =====\n");
        printf("  1. Ajouter un client\n");
        printf("  2. Modifier un client\n");
        printf("  3. Supprimer un client\n");
        printf("  4. Afficher la liste des clients\n");
        printf("  0. Retour au menu principal\n");
        choix = lireChoix();

        switch (choix)
        {
        case 1:
            printf("Fonction ajouter client à compléter.\n");
            break;

        case 2:
            printf("Fonction modifier client à compléter.\n");
            break;

        case 3:
            printf("Fonction supprimer client à compléter.\n");
            break;
        case 4:
            printf("Fonction supprimer client à compléter.\n");
            break;
        case 0:                         break;
        default: printf("  [!] Choix invalide.\n"); break;
        }
    } while (choix != 0);
}

static void menuCommandes(Boutique *boutique)
{
    int choix;
    do
    {
        printf("\n===== GESTION DES COMMANDES =====\n");
        printf("  1. Creer une commande\n");
        printf("  2. Afficher une commande\n");
        printf("  0. Retour au menu principal\n");
        choix = lireChoix();

        switch (choix)
        {
            case 1: menuCreerCommande(boutique); break;
            case 2: afficherUneCommande(boutique); break;
            case 0:                        break;
            default: printf("  [!] Choix invalide.\n"); break;
        }
    } while (choix != 0);
}

static void menuStatistiques(Boutique *b)
{
    int choix;
    do
    {
        printf("\n===== STATISTIQUES =====\n");
        printf("  1. Chiffre d'affaires (total / mois / annee)\n");
        printf("  2. Palmares produits (top 5)\n");
        printf("  3. Palmares clients\n");
        printf("  4. Alertes de stock\n");
        printf("  0. Retour au menu principal\n");
        choix = lireChoix();

        switch (choix)
        {
        case 1: afficherChiffreAffaires(b);  break;
        case 2: afficherPalmaresProduits(b); break;
        case 3: afficherPalmaresClients(b);  break;
        case 4: afficherAlertesStock(b);     break;
        case 0:                              break;
        default: printf("  [!] Choix invalide.\n"); break;
        }
    } while (choix != 0);
}

/* ==========================================================================
 * PROGRAMME PRINCIPAL
 * ========================================================================== */
int main(void)
{
    int choix;

    /* Création et initialisation de la boutique */
    Boutique boutique = {0};

    strcpy(boutique.nom, "ShopFlow");

    boutique.produits = malloc(10 * sizeof(Produit));
    boutique.nbProduits = 0;
    boutique.capaciteProduits = 10;

    boutique.clients = malloc(8 * sizeof(Client));
    boutique.nbClients = 0;
    boutique.capaciteClients = 8;

    boutique.numeroSuivant = 8;
    boutique.idClientSuivant = 1007;

    printf("==============================================\n");
    printf("  SHOPFLOW - Boutique en ligne\n");
    printf("==============================================\n");

    do
    {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("  1. Gestion des produits\n");
        printf("  2. Gestion des clients\n");
        printf("  3. Gestion des commandes\n");
        printf("  4. Statistiques\n");
        printf("  0. Quitter\n");

        choix = lireChoix();

        switch (choix)
        {
        case 1: menuProduits(&boutique); break;
        case 2: menuClients(); break;
        case 3: menuCommandes(&boutique); break;
        case 4: menuStatistiques(&boutique); break;
        case 0: printf("\n  Au revoir !\n"); break;
        default: printf("  [!] Choix invalide.\n"); break;
        }

    } while (choix != 0);

    /* Libération de la mémoire */
    free(boutique.produits);
    free(boutique.clients);

    return EXIT_SUCCESS;
}
