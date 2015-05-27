#!/bin/bash
 
################################################################################
#Florent Pavageau
#15/04/2008
#
# Ce script liste dans le fichier periph_usb.listle nom, le chemin d'acces et 
#+la taille de tous les peripheriques de stockage USB de la machine
################################################################################

Periph_folder=/dev/
Liste_Periph=/dev/disk/by-id


set -x

#ecrit dans le fichier periph_usb.list les peripheriques USB (sans leurs partitions ex:/sdb1,/sdb2)
ls -l "$Liste_Periph" | grep  usb | grep /*[^0-9]$ >/var/lib/installFrogLinux/periph_usb.list 

if [ $(wc -l /var/lib/installFrogLinux/periph_usb.list | cut -d' ' -f1) -eq 0 ]
then	
	echo "pas de peripherique usb "
	exit 2
fi

# on extrait le nom et le chemin de chaue peripherique
set `awk '{print $8, $10}' /var/lib/installFrogLinux/periph_usb.list`
####set `awk '{print $9, $11}' /var/lib/installFrogLinux/periph_usb.list`
#on vide le fichier periph_usb.list
:>/var/lib/installFrogLinux/periph_usb.list

i=1
nbdisques="$#"
disques="$*"
#on recupere les peripherique usb (nom,chemin,taille) dans le fichier periph_usb.list
while [ "$i" -le "$nbdisques" ]
do
	#on recupere le nom dans la variable $nom_cle
	nom_cle=`echo $disques | cut -d ' '  -f"$i"`	
	((i++))
	#on recupere son chemin dacces dans /dev (ex /sdb) dans $chemin_cle
	chemin_cle=`echo  $disques | cut -d ' '  -f"$i" | cut -d/ -f3`

	trouve=0
	for j in "1" "2" "3" "4"
	do
	
	if [ $trouve -eq 0 ]
	then	
		if [ "$(vol_id -t $Periph_folder$chemin_cle$j 2>/dev/null)" = "ext3" ]
		then
		
			uuid_disk=$(vol_id -u $Periph_folder$chemin_cle$j 2>/dev/null) 

			if [ $(echo $?) -eq 0 ]
			then
				if [ $(cat /proc/mounts | grep ^/dev/disk/by-uuid/"$uuid_disk"' '/' '|wc -l) -eq 1 ]
				then
					echo "$chemin_cle$j est monte sur /"
					trouve=1
				else
					echo " $chemin_cle$j pas sur slash"
					
							
	
				fi
			fi		
		fi
	fi
	done

	if [ $trouve -eq 0 ]
	then
		#on stock dans taille_usb.tmp la taille du peripherique	
		fdisk -l "$Periph_folder""$chemin_cle" | grep "$chemin_cle": | tr -d , | cut -d ' ' -f3-4 > /var/lib/installFrogLinux/taille_usb.tmp 
		nb_ligne=`wc -l /var/lib/installFrogLinux/taille_usb.tmp | cut -d ' ' -f1`
		# si le fichier est vide cela signifie que le peripherique n'est pas une cle ou un disque usb
		if [ $nb_ligne -gt 0 ]
		then
			#on n'ecrit donc dans  periph_usb.list que les disque ou cle usb
			((i--))	
			echo -n $nom_cle,>>/var/lib/installFrogLinux/periph_usb.list		
			((i++))
			echo -n $chemin_cle, >>/var/lib/installFrogLinux/periph_usb.list
			cat /var/lib/installFrogLinux/taille_usb.tmp>>/var/lib/installFrogLinux/periph_usb.list
		fi		
	fi
	((i++))	

done
#on allege le nom des peripheriques
sed "s/_[0-9][0-9]*.[^,]*//g" </var/lib/installFrogLinux/periph_usb.list >/var/lib/installFrogLinux/copie.tmp
cat /var/lib/installFrogLinux/copie.tmp>/var/lib/installFrogLinux/periph_usb.list

#on ordonne par taille croissante des disques
sort -n /var/lib/installFrogLinux/periph_usb.list>/var/lib/installFrogLinux/copie.tmp
cat /var/lib/installFrogLinux/copie.tmp>/var/lib/installFrogLinux/periph_usb.list;
#supression des fichiers temporaires
rm /var/lib/installFrogLinux/*.tmp

exit
