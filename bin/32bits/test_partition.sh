#!/bin/bash

################################
#Florent Pavageau
#06/05/2008
#
# Pour l'installation guidee sur disque rigide :
# verifie qu'il y a 2 partitions de crees au maximun
#+et au'il reste au moins 8go non formate
#
##

declare -r CLE=$(cat /var/lib/installFrogLinux/disque_cle.list | cut -d, -f2)
declare -r PERIPH_FOLDER=/dev/

# on verifie au'il n'y a pas plus de 2 partions presentes
nb_partitions=$(fdisk -l $PERIPH_FOLDER$CLE | grep ^$PERIPH_FOLDER[^' ']*[1234]  |wc -l)
#echo $nb_partitions
echo "nb partitions : $nb_partitions"	 


set -x

if [ "$nb_partitions" -le 2 ]
then
	#taille de la cle en Ko
	taille_cle=$(fdisk -s $PERIPH_FOLDER$CLE)
	taille_prise=0

	##########################################
	#   cas ou il ya 1 ou 2 partitions       #
	##########################################
	if [ "$nb_partitions" -gt 0 ]
	then	
		
		fdisk -l $PERIPH_FOLDER$CLE | grep ^$PERIPH_FOLDER[^' ']*[1234] | tr -d \* | tr -s [:blank:] ' '>/var/lib/installFrogLinux/partitions.lst
		set `cat partitions.lst | tr -d + | cut -d' ' -f4`
		for i in "$@"
			do
				#echo "i = $i"
				taille_prise=$(echo "$taille_prise+$i"|bc)
			done	
	
		echo "taille prise : $taille_prise"
		
		# on test si il ya 8Go libre	
		if [ $(echo "$taille_cle-$taille_prise"|bc) -lt 8000000 ]
		then
			echo "espace insuffisant : Froglinux a besoin d'un espace libre de 8Go pour être installe"
			exit 2
		fi 
	
		echo  "espace suffisant"
	
		#####################################################	
		#on verifie qu'il y a un espace libre contigu de 8Go#	
		#####################################################
	
		#unite d,un cylindre en Ko
		unite_cylindre=$(fdisk -l $PERIPH_FOLDER$CLE | grep ^Unit |cut -d= -f3 | cut -d' ' -f2)
		unite_cylindre=$(echo "$unite_cylindre/1024"|bc)
		echo "unite cylindre : $unite_cylindre"
		
		#nombre de cylindres requis pour avoir un espace >=8Go
		nb_cylindre_requis=$(echo " 8000000/$unite_cylindre"|bc)
		echo $nb_cylindre_requis

		nb_cylindre=$(fdisk -l $PERIPH_FOLDER$CLE |grep cylind[er][er]s$ | cut -d, -f3 |cut -d' ' -f2)
		i=1
		delim_part=$(cat /var/lib/installFrogLinux/partitions.lst | cut -d' ' -f2-3)
		nb_delim_part=$(echo $delim_part | wc -w)
		#echo $delim_part

		#si un espace vide de 8Go existe trouve=1
		trouve=0
		while [ $i -le $nb_delim_part ]
		do
			#espace non trouve
			if [ "$trouve" -eq 0 ]
			then
				debut=$(echo $delim_part | cut -d' ' -f"$i" )
				#debut premiere partition
				if [ $i -eq 1 ]
				then
					if [ $debut -ge $nb_cylindre_requis ]
					then
						echo " espace avant parition suffisament grande"	
						trouve=1
						echo "1 126" >/var/lib/installFrogLinux/deb_partition.txt
						exit 0
					fi
				#fin derniere partition
				else
					fin=$(echo $delim_part | cut -d' ' -f"$i" )
					fin=$(($fin+1)) 
					if [ "$i" -eq "$nb_delim_part" ]
					then
						if [ $(echo "$nb_cylindre-$fin" |bc) -ge $nb_cylindre_requis ]
						then
							echo "place suffisante apres derniere partition"
							trouve=1
							deb_ext3=$(echo "$fin+125"|bc)
							echo $fin $deb_ext3 >/var/lib/installFrogLinux/deb_partition.txt
							exit 0
						fi
					#espace entre 2 partitions
					else
						((i++))
						debut=$(echo $delim_part | cut -d' ' -f"$i" )
						debut=$(($debut-1))
						if [ $(echo "$debut-$fin"|bc) -ge $nb_cylindre_requis ]
						then
							echo "espace entre 2 partitions > 8Go"
							trouve=1
							deb_ext3=$(echo "$fin+125"|bc)
							echo $fin $deb_ext3 >/var/lib/installFrogLinux/deb_partition.txt
							exit 0
						fi
					fi
				fi			
			#espace trouve
			else
				exit 0
				
			fi
			((i++))


		done

		if [ $trouve -eq 0 ]
		then
			echo "pas de partitions dispo"
			exit 2
		fi

	else
		echo "pas de partitions"
		# on test si il ya 8Go libre	
		if [ "$taille_cle" -lt 8000000 ]
		then
			echo "espace insuffisant : Froglinux a besoin d'un espace libre de 8Go pour être installe"
			exit 2
		fi 
		echo "1 126" >/var/lib/installFrogLinux/deb_partition.txt
		############################################################
		# le disque n'a pas de partitions et a suffisamment de place
		
		echo  "espace suffisant"
		exit 0
	fi
	
else 
	echo "Vous devez avoir 2 partitions au maximun"
	exit 2	
	
fi
rm /var/lib/installFrogLinux/num_partition.list /var/lib/installFrogLinux/deb_partition.txt
exit 
