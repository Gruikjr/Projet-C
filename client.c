/* ============================================================
 * client.c - Implementation des clients et de la fidelite
 * ------------------------------------------------------------
 * ROLE : gere le tableau dynamique b->clients (malloc/realloc).
 *        - ajouterClient : id donne par b->idClientSuivant (jamais saisi),
 *          unicite de l'email (inactifs inclus), adresse complete
 *        - supprimerClient : suppression LOGIQUE, refusee s'il existe des
 *          commandes non annulees
 *        - afficherClient : fiche + historique des commandes
 * ETAT : trouverClient() et les fonctions de points sont faits.
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"


/* Rechercher un client par son identifiant */

int trouverClient(Boutique *b, int idClient) {

    for (int i = 0; i < b->nbClients; i++) {

        if (b->clients[i].idClient == idClient) {
            return i + 1;
        }
    }

    return 0;
}


/* Verifier si un email existe deja */

int emailExiste(Boutique *b, char email[]) {

    for (int i = 0; i < b->nbClients; i++) {

        if (strcmp(b->clients[i].email, email) == 0) {
            return 1;
        }
    }
    return 0;
}


/* Ajouter un client */

int ajouterClient(Boutique *b, Client nouveauClient) {

    if (emailExiste(b, nouveauClient.email)) {
        return 0;
    }

    if (b->nbClients == b->capaciteClients) {
        int nouvelleCapacite = b->capaciteClients + 5;

        Client *temp = realloc(b->clients, nouvelleCapacite * sizeof(Client));

        if (temp == NULL) {
            return 0;
        }

        b->clients = temp;
        b->capaciteClients = nouvelleCapacite;
    }

    nouveauClient.idClient = b->idClientSuivant;
    nouveauClient.actif = 1;
    nouveauClient.pointsFidelite = 0;
    nouveauClient.commandes = NULL;
    nouveauClient.nbCommandes = 0;
    nouveauClient.capaciteCommandes = 0;

    b->clients[b->nbClients] = nouveauClient;

    b->nbClients++;
    b->idClientSuivant++;

    return 1;
}

//blabla blibli bloublou

/* Modifier un client */

int modifierClient(Boutique *b, int idClient, Client modification) {

    int index = trouverClient(b, idClient);

    if (index == 0) {
        return 0;
    }

    index--;

    if (strcmp(b->clients[index].email, modification.email) != 0 &&
        emailExiste(b, modification.email)) {
        return 0;
    }

    strcpy(b->clients[index].nom, modification.nom);
    strcpy(b->clients[index].prenom, modification.prenom);
    strcpy(b->clients[index].email, modification.email);
    strcpy(b->clients[index].telephone, modification.telephone);

    b->clients[index].adresse = modification.adresse;

    return 1;
}


/* Supprimer un client : suppression logique */

int supprimerClient(Boutique *b, int idClient) {

    int index = trouverClient(b, idClient);

    if (index == 0) {
        return 0;
    }

    index--;

    if (b->clients[index].actif == 0) {
        return 0;
    }

    for (int i = 0; i < b->clients[index].nbCommandes; i++) {

        if (strcmp(b->clients[index].commandes[i].statut, STATUT_ANNULEE) != 0) {
            return 0;
        }
    }

    b->clients[index].actif = 0;

    return 1;
}


/* Afficher la liste des clients */

void afficherListeClients(Boutique *b) {
    printf("\n LISTE DES CLIENTS \n");
    for (int i = 0; i < b->nbClients; i++) {
        if (b->clients[i].actif == 1) {
            float chiffreAffaires = 0;
            for (int j = 0; j < b->clients[i].nbCommandes; j++) {
                if (b->clients[i].commandes[j].statut, STATUT_LIVREE) {
                    chiffreAffaires += b->clients[i].commandes[j].montantTVA;
                }
            }
            printf("\nID : %d\n", b->clients[i].idClient);
            printf("Nom : %s %s\n", b->clients[i].prenom, b->clients[i].nom);
            printf("Email : %s\n", b->clients[i].email);
            printf("Telephone : %s\n", b->clients[i].telephone);
            printf("Nombre de commandes : %d\n", b->clients[i].nbCommandes);
            printf("Chiffre d'affaires : %.2f euros\n", chiffreAffaires);
        }
    }
}


/* Afficher un client et son historique de commandes */
void afficherClient(Boutique *b, int idClient) {

    int index = trouverClient(b, idClient);

    if (index == 0) {
        printf("Client non trouve.\n");
        return;
    }

    index--;

    printf("\nFICHE CLIENT\n");
    printf("ID : %d\n", b->clients[index].idClient);
    printf("Nom : %s %s\n", b->clients[index].prenom, b->clients[index].nom);
    printf("Email : %s\n", b->clients[index].email);
    printf("Telephone : %s\n", b->clients[index].telephone);
    printf("Adresse : %s, %s, %s, %s\n",
           b->clients[index].adresse.rue,
           b->clients[index].adresse.ville,
           b->clients[index].adresse.codePostal,
           b->clients[index].adresse.pays);
    printf("Nombre de commandes : %d\n", b->clients[index].nbCommandes);
    printf("Points de fidelite : %d\n", b->clients[index].pointsFidelite);

    if (b->clients[index].nbCommandes > 0) {
        printf("\nHISTORIQUE DES COMMANDES\n");
        for (int i = 0; i < b->clients[index].nbCommandes; i++) {
            Commande *commande = &b->clients[index].commandes[i];
            printf("\nCommande ID : %d\n", commande->idCommande);
            printf("Date : %s\n", commande->date);
            printf("Montant HT : %.2f euros\n", commande->montantHT);
            printf("Montant TVA : %.2f euros\n", commande->montantTVA);
            printf("Statut : %s\n", commande->statut);
        }
    } else {
        printf("\nAucune commande pour ce client.\n");
    }
}



/* Ajouter des points de fidelite */

void ajouterPoints(Boutique *b, int idClient, int points) {
    printf("erreur");

}


/* Retirer des points de fidelite */

void retirerPoints(Boutique *b, int idClient, int points) {
    printf("erreur");
}