# ============================================================
# Makefile - Compilation du projet ShopFlow
# ------------------------------------------------------------
# make          : compile tous les modules et cree l'executable 'shopflow'
# make valgrind : lance le programme sous valgrind (detection des fuites)
# make clean    : supprime les .o, .d et l'executable
# Options -Wall -Wextra : exigees par le sujet (zero avertissement).
# -MMD : genere les dependances .h (modifier types.h recompile les .c).
# ============================================================
CC      = gcc
CFLAGS  = -Wall -Wextra -g -MMD
TARGET  = shopflow
SRC     = main.c produit.c client.c commande.c statistique.c utils.c initialisation.c
OBJ     = $(SRC:.c=.o)
DEP     = $(SRC:.c=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

.PHONY: all clean valgrind
