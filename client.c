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
#include "utils.h"


/* Rechercher un client ACTIF par son identifiant : retourne sa position + 1,
 * ou 0 s'il est introuvable (ou supprime logiquement). */

int trouverClient(Boutique *b, int idClient) {

    for (int i = 0; i < b->nbClients; i++) {

        if (b->clients[i].idClient == idClient && b->clients[i].actif == 1) {
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


/* Afficher la liste des clients (CA HT hors commandes annulees) */

void afficherListeClients(Boutique *b) {
    printf("\n=== LISTE DES CLIENTS ===\n");
    printf("%-4s %-26s %-28s %5s %12s\n",
           "ID", "Nom", "Email", "Cmd", "CA HT (EUR)");
    printf("------------------------------------------------------------------------------\n");

    int affiches = 0;
    for (int i = 0; i < b->nbClients; i++) {
        const Client *cl = &b->clients[i];
        if (cl->actif != 1) continue;

        double chiffreAffaires = 0.0;
        for (int j = 0; j < cl->nbCommandes; j++) {
            if (strcmp(cl->commandes[j].statut, STATUT_ANNULEE) != 0)
                chiffreAffaires += cl->commandes[j].montantHT;
        }

        char nomComplet[MAX_NOM + MAX_PRENOM + 2];
        snprintf(nomComplet, sizeof nomComplet, "%s %s", cl->prenom, cl->nom);
        printf("%-4d %-26.26s %-28.28s %5d %12.2f\n", cl->idClient,
               nomComplet, cl->email, cl->nbCommandes, chiffreAffaires);
        affiches++;
    }
    if (affiches == 0) printf("(aucun client)\n");
}


/* Afficher un client et son historique de commandes */
void afficherClient(Boutique *b, int idClient) {
    int index = trouverClient(b, idClient);

    if (index == 0) {
        printf("Client non trouve.\n");
        return;
    }

    index--;

    printf("\n FICHE CLIENT \n");
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

    printf("\n HISTORIQUE DES COMMANDES \n");
    for (int i = 0; i < b->clients[index].nbCommandes; i++) {
        Commande commande = b->clients[index].commandes[i];
        printf("\nCommande ID : %d\n", commande.numero);
        printf("Date : %02d/%02d/%04d\n",
               commande.dateCommande.jour,
               commande.dateCommande.mois,
               commande.dateCommande.annee);
        printf("Montant TTC : %.2f euros\n", commande.montantTTC);
        printf("Statut : %s\n", commande.statut);
    }
}


/* Ajouter des points de fidelite */

void ajouterPoints(Boutique *b, int idClient, int points) {
    int index = trouverClient(b, idClient);

    if (index == 0) {
        printf("Client non trouve.\n");
        return;
    }

    index--;

    b->clients[index].pointsFidelite += points;

}


/* Retirer des points de fidelite */

void retirerPoints(Boutique *b, int idClient, int points) {
    int index = trouverClient(b, idClient);

    if (index == 0) {
        printf("Client non trouve.\n");
        return;
    }

    index--;

    if (b->clients[index].pointsFidelite < points) {
        b->clients[index].pointsFidelite = 0;
    } else {
        b->clients[index].pointsFidelite -= points;
    }
}

/* Saisie complete d'un client (utilisee pour l'ajout et la modification).
 * Tous les champs sont valides : non vides, email avec '@', telephone a
 * 10 chiffres, code postal a 5 chiffres. */
Client saisirClient(void)
{
    Client c;
    memset(&c, 0, sizeof c);

    lireChaine("  Nom : ",    c.nom,    sizeof c.nom);
    lireChaine("  Prenom : ", c.prenom, sizeof c.prenom);
    lireChaineValide("  Email : ", c.email, sizeof c.email,
                      estEmailValide, "adresse email invalide (ex. a@b.fr).");
    lireChaineValide("  Telephone (10 chiffres) : ", c.telephone,
                     sizeof c.telephone, estTelephoneValide,
                     "10 chiffres attendus.");
    lireChaine("  Rue : ",   c.adresse.rue,   sizeof c.adresse.rue);
    lireChaine("  Ville : ", c.adresse.ville, sizeof c.adresse.ville);
    lireChaineValide("  Code postal (5 chiffres) : ", c.adresse.codePostal,
                     sizeof c.adresse.codePostal, estCodePostalValide,
                     "5 chiffres attendus.");
    lireChaine("  Pays : ",  c.adresse.pays,  sizeof c.adresse.pays);
    return c;
}
