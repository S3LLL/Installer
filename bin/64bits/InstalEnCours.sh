#!/bin/bash

#########################################
# 23 mai 2008
# Florent Pavageau
# Ce script verifie si l'installer est deja lance,
#+ auquel cas une fenetre d'erreur sinon l'installer 
#+ est lancé
##

if [ $(ps -e |grep installer|wc -l) -eq 0 ]
then
	gksu /var/lib/installFrogLinux/installer 
	exit
else
/var/lib/installFrogLinux/msgErreur "
Une installation de FrogLinux est déjà en cours, 
vous devez attendre qu'elle soit terminée
pour en lancer une nouvelle.
"
exit 2
fi
exit
