#! /bin/bash

###################################################
# Florent Pavageau
# 14/05/2008
#
# Ce script donne les numero des partitions disponibles
#+pour l'installation guidee

declare -r CLE=$(cat /var/lib/installFrogLinux/disque_cle.list | cut -d, -f2)
declare -r PERIPH_FOLDER=/dev/

:>/var/lib/installFrogLinux/num_partition.list

# lorsque les 2 numero de partitions sont trouvees
#+ trouve=2
trouve=0

for i in 1 2 3 4
do
	# si le disque n' a pas de partition numero $i (i.e wc -l =0) alors ce numero est dispo
	if [ $(fdisk -l $PERIPH_FOLDER$CLE | grep ^/dev/[^' ']*.$i | wc -l) -eq 0 ]
	then
		#echo $i
		echo -n "$i "  >> /var/lib/installFrogLinux/num_partition.list
		trouve=$(echo "$trouve+1"|bc)
	fi

	if [ $trouve -eq 2 ]
	then
		echo "les numero de partitions ont ete trouvees"
		cat /var/lib/installFrogLinux/num_partition.list
		exit
	fi

done
if [ $trouve -lt 2 ]
	then
		echo "erreur : les numeros de partitions n'ont pu être trouvees"
		exit 2
	fi

exit
