# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS =

# Noms des exécutables
SERVER_EXEC = serveur
CLIENT_EXEC = client
SCRIPT = script.sh

# Fichiers source
SERVER_SRC = serveur.c
CLIENT_SRC = client.c

# Fichiers objets (générés à partir des sources)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

# Cible par défaut
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Compilation du serveur
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation du client
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Règle pour générer les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC) $(SERVER_OBJ) $(CLIENT_OBJ)

# Lancement du script
run: all
	./$(SCRIPT)

# Phony targets (pour éviter les conflits avec des fichiers du même nom)
.PHONY: all clean run
