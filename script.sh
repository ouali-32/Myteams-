#!/bin/bash

# Compilation
gcc serveur.c -o serveur && gcc client.c -o client
if [ $? -ne 0 ]; then
    echo "Erreur lors de la compilation."
    exit 1
fi

# Lancer le serveur
./serveur 4242 &
SERVER_PID=$!
sleep 2

# Lancer plusieurs clients
for i in {1..10}; do
    read -p "Entrez votre pseudo pour le client $i : " username
    gnome-terminal -- ./client 127.0.0.1 4242 "$username" &
done

# Attendre que les clients se terminent
wait

# Nettoyage
kill $SERVER_PID
rm -f serveur client
