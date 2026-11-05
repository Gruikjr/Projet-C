# ShopFlow - Gestion d'une boutique en ligne (XTI101)

Projet C en binome, modulaire, 100 % en memoire (malloc / realloc / free).

## Compilation et lancement
    make            # gcc -Wall -Wextra : zero avertissement
    ./shopflow
    make valgrind   # zero fuite

## Modules
produit, client, commande, statistique, utils, initialisation, main (menus), types.h.

## Regles metier
Remise sur le HT brut (0/5/10/15 %), TVA 20 % sur le HT remise, port 5,90 EUR offert
des 60 EUR HT brut, 1 point de fidelite par tranche de 10 EUR TTC, stock decremente a
l'ajout d'une ligne et reintegre a la suppression d'une ligne ou a l'annulation.

## Fonctionnalites creatives
1. Statistiques par categorie (menu Statistiques > 5)
2. Export CSV des commandes (menu Statistiques > 6, fichier commandes.csv)
