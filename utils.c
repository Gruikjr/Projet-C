/* ============================================================
 * utils.c - Implementation de la saisie et des outils chaines
 * ------------------------------------------------------------
 * ROLE : evite scanf direct (dangereux) en utilisant fgets + conversion.
 *        Verifie les intervalles et refuse les chaines vides.
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "types.h"
#include "utils.h"

/* ============================================================
 * SAISIE SECURISEE
 * ============================================================ */

static int lireLigne(char *dest, size_t taille)
{
    char tampon[512];

    if (fgets(tampon, sizeof tampon, stdin) == NULL) {
        fprintf(stderr, "\nFin de saisie (EOF) : arret du programme.\n");
        exit(EXIT_FAILURE);
    }

    size_t n = strlen(tampon);
    if (n > 0 && tampon[n - 1] == '\n') {
        tampon[--n] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
        return 0;
    }

    if (n >= taille) return 0;
    strcpy(dest, tampon);
    return 1;
}

void lireChaine(const char *invite, char *dest, size_t taille)
{
    for (;;) {
        printf("%s", invite);
        if (!lireLigne(dest, taille)) {
            printf("Erreur : saisie trop longue (%zu caracteres max).\n",
                   taille - 1);
            continue;
        }

        if (dest[strspn(dest, " \t")] == '\0') {
            printf("Erreur : la saisie ne peut pas etre vide.\n");
            continue;
        }
        return;
    }
}

int lireEntier(const char *invite, int min, int max)
{
    char buf[64];

    for (;;) {
        printf("%s", invite);
        if (!lireLigne(buf, sizeof buf)) {
            printf("Saisie invalide.\n");
            continue;
        }

        char *fin;
        errno = 0;
        long valeur = strtol(buf, &fin, 10);

        if (fin == buf || *fin != '\0' || errno == ERANGE ||
            valeur < min || valeur > max) {
            printf("Erreur : entier compris entre %d et %d attendu.\n",
                   min, max);
            continue;
        }
        return (int)valeur;
    }
}

double lireReel(const char *invite, double min, int minExclus)
{
    char buf[64];

    for (;;) {
        printf("%s", invite);
        if (!lireLigne(buf, sizeof buf)) {
            printf("Saisie invalide.\n");
            continue;
        }

        for (char *p = buf; *p; ++p) {
            if (*p == ',') *p = '.';
        }

        char *fin;
        errno = 0;
        double valeur = strtod(buf, &fin);

        if (fin == buf || *fin != '\0' || errno == ERANGE ||
            !isfinite(valeur) ||
            valeur < min || (minExclus && valeur <= min) ||
            valeur > 1e9) {
            printf("Erreur : nombre %s %.2f attendu.\n",
                   minExclus ? "strictement superieur a"
                             : "superieur ou egal a", min);
            continue;
        }
        return valeur;
    }
}

/* ============================================================
 * OUTILS CHAINES
 * ============================================================ */

void versMinuscules(char *dest, const char *src, size_t taille)
{
    size_t i;
    for (i = 0; i + 1 < taille && src[i] != '\0'; ++i) {
        dest[i] = (char)tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

int contientMotCle(const char *texte, const char *mot)
{
    char texteMin[LG_DESIGNATION];
    char motMin[LG_DESIGNATION];

    versMinuscules(texteMin, texte, sizeof texteMin);
    versMinuscules(motMin, mot, sizeof motMin);
    return strstr(texteMin, motMin) != NULL;
}
