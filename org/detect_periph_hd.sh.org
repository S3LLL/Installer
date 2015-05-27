#!/bin/bash
 
################################################################################
#Florent Pavageau 15/04/2008
#Damien Simet	05/2008
#
#
# Ce script liste dans le fichier periph_HD.list le nom, le chemin d'acces et 
# la taille de tous les disques rigides de la machines
################################################################################
PERIPH_FOLDER=/dev/
LISTE_HD=/dev/disk/by-id

# On ecrit dans le fichier periph_HD.list les disques rigides (sans leurs partitions ex:/sdb1,/sdb2)
# Exemple : lrwxrwxrwx 1 root root  9 2008-05-06 10:20 scsi-1ATA_SAMSUNG_HD160JJP_S0DFJ1HL444739 -> ../../sda
ls -l "$LISTE_HD" | grep  scsi | grep /*[^0-9]$ >/var/lib/installFrogLinux/periph_HD.list 

# On extrait le nom et le chemin de chacun des peripheriques
# Exemple : scsi-1ATA_SAMSUNG_HD160JJP_S0DFJ1HL444739 ../../sda
set `awk '{print $8, $10}' /var/lib/installFrogLinux/periph_HD.list`
# On vide le fichier periph_HD.list
>/var/lib/installFrogLinux/periph_HD.list

# On recupere le (nom,chemin,taille) des disques rigides
i=1
nbdisques="$#"
disques="$*"
while [ "$i" -le "$nbdisques" ] ; do
#set -x
	# Nom complet du disque
	# Exemple : scsi-1ATA_SAMSUNG_HD160JJP_S0DFJ1HL444739
	NOM_HD=`echo $disques | cut -d ' '  -f"$i"`	
	((i++))
	#Son chemin d'acces dans /dev
	# Exemple : /sda
	CHEMIN_HD=`echo  $disques | cut -d ' '  -f"$i" | cut -d/ -f3`

	trouve=0
	for j in "1" "2" "3" "4"
	do
	
	if [ $trouve -eq 0 ]
	then	
		if [ $(vol_id -t $PERIPH_FOLDER$CHEMIN_HD$j 2>/dev/null) = "ext3" ]
		then
			uuid_disk=$(vol_id -u $PERIPH_FOLDER$CHEMIN_HD$j 2>/dev/null) 

			if [ $(echo $?) -eq 0 ]
			then
				if [ $(cat /proc/mounts | grep ^/dev/disk/by-uuid/"$uuid_disk"' '/' '|wc -l) -eq 1 ]
				then
					echo " $CHEMIN_HD$j est monte sur /"
					trouve=1
				else
					echo " $CHEMIN_HD$j pas sur slash"	
	
				fi
			fi
		fi
	fi
	done

	if [ $trouve -eq 0 ]
	then
		#on stock dans TAILLE_HD.tmp la taille du peripherique	
		fdisk -l "$PERIPH_FOLDER""$CHEMIN_HD" | grep "$CHEMIN_HD": | tr -d , | cut -d ' ' -f3-4 > /var/lib/installFrogLinux/TAILLE_HD.tmp 
		# On reformate le fichier
		# Exemple : scsi-1ATA_SAMSUNG_HD160JJP_S0DFJ1HL444739,sda,160.0 Go
		((i--))	
		echo -n $NOM_HD,>>/var/lib/installFrogLinux/periph_HD.list		
		((i++))
		echo -n $CHEMIN_HD, >>/var/lib/installFrogLinux/periph_HD.list
		cat /var/lib/installFrogLinux/TAILLE_HD.tmp>>/var/lib/installFrogLinux/periph_HD.list		
	fi
	((i++))	
	
done
# Supression des fichiers temporaires
rm /var/lib/installFrogLinux/*.tmp

exit
