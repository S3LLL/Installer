#!/bin/bash

################################################################################
#
#Damien Simet	05/2008
#Florent Pavageau 15/05/2008
# Ce script installe FrogLinux sur un disque rigide
# MODE AUTO : TOUT LE CONTENU DU DISQUE OU SERA EFFECTUE L'INSTALLATION SERA DETRUIT
#
################################################################################

echo "################################################################"
echo "#                            INSTALL_HD.SH                     #"
echo "################################################################"

declare -r PERIPH_FOLDER=/dev/
declare -r DISQUE=$(cat /var/lib/installFrogLinux/liste_disque_rigide | cut -d, -f2)
declare -r TAILLE_DISQUE=$(cat /var/lib/installFrogLinux/liste_disque_rigide | cut -d, -f3)
#Création de l'espace disque pour l'installation se fait par l'appelle de Gparted en mode guide et destruction en mode auto
# TODO FAIRE UN TEST DE LA TAILLE MINI pour la version guide et AUTO 

CHEMIN=$PERIPH_FOLDER$DISQUE
echo " Disque choisi : $CHEMIN , $TAILLE_DISQUE"
touch /var/lib/installFrogLinux/fichiers_exclus_utilisateur.list

if [ "$#" -eq 1 ]
then
	if [ $1 = "guidee" ]
	then
		SWAP=$(cat /var/lib/installFrogLinux/num_partition.list | cut -d' ' -f1)
		PRIMAIRE=$(cat /var/lib/installFrogLinux/num_partition.list | cut -d' ' -f2)
		echo "		:: numero Swap : $SWAP, Primaire : $PRIMAIRE"
		if [ $(wc -w /var/lib/installFrogLinux/deb_partition.txt | cut -d' ' -f1) -eq 2 ]
		then		
			DEBSWAP=$(cat /var/lib/installFrogLinux/deb_partition.txt |cut -d' ' -f1) 
			DEBPRIMAIRE=$(cat /var/lib/installFrogLinux/deb_partition.txt |cut -d' ' -f2) 
			echo "		:: debut Swap : $DEBSWAP, Primaire : $DEBPRIMAIRE"
		else
			echo "erreur lecture debut partitions"
			exit 2
		fi
	else
		echo "erreur argument"
		exit 2
		
	fi
else
	SWAP=1
	PRIMAIRE=2
	DEBSWAP=""
	DEBPRIMAIRE=""
		> /var/lib/installFrogLinux/mbr_null
	i=1
	while [ $i -le 5120 ] ; do
		echo -n 0 >> /var/lib/installFrogLinux/mbr_null
		((i++))
	done

	dd if=/var/lib/installFrogLinux/mbr_null of=$CHEMIN 2> /dev/null > /dev/null 
	rm /var/lib/installFrogLinux/mbr_null

fi


# TODO creation des partitions pour un HD
#Creation des partitions (deux partitions primaires)
function creer_partition(){
# SWAP 1024Mo et Linux lla place restante
# avec redirection des messages dans la poubelle
# TODO pour le mode guide changer le numero du swap et primaire


echo "-------------CREATION DES PARTITIONS-------------"

sync
sync

if [ $(fdisk -l $CHEMIN |grep ^/dev |wc -l | cut -d' ' -f1) -lt 2 ]
then
fdisk $CHEMIN 2>/dev/null >/var/lib/installFrogLinux/fdisk.log<<EOF
n
p
$SWAP
$DEBSWAP
+1024M
n
p
$PRIMAIRE
$DEBPRIMAIRE

t
$SWAP
82
a
$PRIMAIRE
w
q
EOF
else
fdisk $CHEMIN 2>/dev/null >/var/lib/installFrogLinux/fdisk.log<<EOF
n
p
$SWAP
$DEBSWAP
+1024M
n
p
$DEBPRIMAIRE

t
$SWAP
82
a
$PRIMAIRE
w
q
EOF
fi
if [ $? -ne 0 ]
then
	echo "erreur lors de la creation des partitions"
	#exit 2
fi

sleep 5	# nécessaire pour donner le temps au disque

sync
sync
echo "		:: demontage du disque"


if [ $(mount | grep $CHEMIN$PRIMAIRE  |wc -l |cut -d' ' -f1) -eq 1 ]
then
	if [ $(lsof -w $CHEMIN$PRIMAIRE | wc -l | cut -d' ' -f1) -gt 0 ]	
	then
		for i in $(lsof -w $CHEMIN$PRIMAIRE |grep ^[^COMMAND] |tr -s [:blank:] ' ' | cut -d' ' -f2 | uniq)
		do
			kill -9 "$i"
		done
	fi

	umount  -l $CHEMIN$PRIMAIRE  
fi

}

#Creation des systemes de fichiers
function creer_systeme_fichier(){
# Creation d'un systeme de fichier sur la partition primaire cree
# On demonte un volume deja existant
# on supprime les processus eventuels pour pouvoir demonter la cle

printf "\n		:: Création du système de fichier Ext2/Ext3\n"

mkfs.ext3 $CHEMIN$PRIMAIRE -L FrogLinux #2>/dev/null >/dev/null













# Creation d'une swap sur l'autre partition primaire
printf "\n		:: Création du Swap\n"
mkswap $CHEMIN$SWAP #>/dev/null
#printf "\n		:: Activation du Swap\n"
#swapon $CHEMIN$SWAP
}

#Montage de la partition primaire
function monter_part_primaire(){
# On supprime le dossier au prealable
rm -fr /dossier_primaire 
mkdir /dossier_primaire 2> /dev/null
#montage de la partition d'installation sur le repertoire /dossier_primaire
echo $CHEMIN$PRIMAIRE
mount $CHEMIN$PRIMAIRE /dossier_primaire  

if [ $? -ne 0 ]; then
	printf "\nIl y a un problème pour monter la partition $CHEMIN$PRIMAIRE.\nVeuillez appeller le service technique.\n"
	exit 2	
fi

}

# Copie du systeme sur le disque
function copie_fichier(){
printf "\n-------------Installation de Froglinux sur le Disque-------------\n"

# TODO Changement du chemin de la liste d' exclusion
# On copie le systeme dans presque sa totalite 
# Des fichiers doivent etre exclu : il se trouve dans fichiers_exclus_base.list 

lien_dvd_live="/bin /boot /lib /opt /sbin /usr"

:>fichiers_exclus_dvd_live.list
echo " remplacement des liens symboliques par les fichiers ciblés"
for i in $lien_dvd_live
do
	if [ $(ls -ld $i | grep ^l |wc -l) -eq 1 ]
	then
		echo "$i" >> 	fichiers_exclus_dvd_live.list
		mkdir /dossier_primaire/$i
		fichier_pointe=$(readlink -f $i)
		rsync -r -o -g -l -p -E $fichier_pointe/* /dossier_primaire/$i/
	fi

done
cat fichiers_exclus_dvd_live.list

if [ $? -ne 0 ]
then
	echo "erreur lors de la copie du systeme"
	#exit 2
fi

rsync -r -o -g -l -p -E --exclude-from=/var/lib/installFrogLinux/fichiers_exclus_base.list --exclude-from=/var/lib/installFrogLinux/fichiers_exclus_dvd_live.list / /dossier_primaire/
if [ $? -ne 0 ]
then
	echo "erreur lors de la copie du systeme"
	#exit 2
fi
# On cree les  fichiers exclus sur la nouvelle installation
echo "		:: creation des fichiers exclus"

for i in $(cat /var/lib/installFrogLinux/fichiers_exclus_base.list | grep [^/dossier_primaire]|grep [^/etc/froglinux/DVDENCOURS])
do
	mkdir /dossier_primaire/"$i"
done
# Cas particulier des dossiers sur la nouvelle installation
# Droits au dossier  /tmp
echo "		:: modifications des droits de /tmp"
chmod 777 /dossier_primaire/tmp
chmod a+t /dossier_primaire/tmp
# Structure vide de /var/log
echo "		:: creation de la structure /var/log"
find /var/log -type d -exec mkdir /dossier_primaire{} \; 
# Il y a un msg indiquant que  /dossier_primaire//var/log existe deja cela est normale
# Om vide mtab
>/dossier_primaire/etc/mtab
}

#Creation des fichiers relatifs au point de montage
function point_de_montage(){
echo "-------------------CREATION FSTAB ET GRUB-------------------"
echo "		:: Recuperation dde l'UUID du Swap et de l'ext3 du disque"

UUIDSLASH=$(vol_id $CHEMIN$PRIMAIRE | grep ^ID_FS_UUID= |cut -d= -f2)
UUIDSWAP=$(vol_id $CHEMIN$SWAP | grep ^ID_FS_UUID= |cut -d= -f2)

echo "			- uuid du Swap : $UUIDSWAP"
echo "			- uuid de l'ext3 : $UUIDSLASH"

echo "		:: Creation du fstab.clean"
#creation du /etc/fstab de base
cat > /dossier_primaire/etc/fstab << EOF
# /etc/fstab:  FrogLinux
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
proc            /proc           proc    defaults        0       0
#$CHEMIN$PRIMAIRE
UUID=$UUIDSLASH	/	ext3	defaults,errors=remount-ro	0	1
#$CHEMIN$SWAP
UUID=$UUIDSWAP	none	swap	sw	0	0
EOF



cp  /dossier_primaire/etc/fstab /dossier_primaire/etc/fstab.clean


#creation du fichier /etc/resolv.conf
>/dossier_primaire/etc/resolv.conf

#creation du fichier menu.lst pour grub
#recuperation du numero de la ligne du disque d'installation dans le fichier /sfdisk_disk
#DISQUE_GRUB=`cat /sfdisk_disk | grep -n $CHEMIN | cut -f1 -d:`
echo "		:: Recuperation du numero du disque et de ses partitions pour le Grub"
DISQUE_GRUB=$(sfdisk -s | grep ^/dev |grep -n $CHEMIN |cut -d: -f1)
#decrementation pour obtenir le numero du disque en notation GRUB
DISQUE_GRUB=`expr $DISQUE_GRUB - 1`
#idem pour le numero de la partition
PARTITION_GRUB=`expr $PRIMAIRE - 1`
echo "			- Disque dans Grub : hd$DISQUE_GRUB,$PARTITION_GRUB" > /titi.txt

echo "		:: Installation du Grub"
#creation du fichier /dossier_pimaire/boot/grub/menu.lst

echo CHEMINPRIMAIRE=$CHEMIN$PRIMAIRE >> /titi.txt
####umount $CHEMIN$PRIMAIRE
#####sleep 2
####grub-install --recheck --no-floppy $CHEMIN$PRIMAIRE
grub-install --no-floppy --recheck --root-directory=/dossier_primaire  $CHEMIN

set +x
cp -r /dev/* /dossier_primaire/dev

set -x
cp -r /boot /dossier_primaire
cp /proc/devices /dossier_primaire/proc

chroot /dossier_primaire grub-mkconfig -o /boot/grub/grub.cfg


####rub-install --recheck --no-floppy --root-directory=/ $CHEMIN$PRIMAIRE 2>> /titi.txt >> /titi.txt
####update-grub -y 2>> /titi.txt >> /titi.txt

cd /
####umount $CHEMIN$PRIMAIRE

###grub --batch <<EOF
###root (hd${DISQUE_GRUB},$PARTITION_GRUB)
###setup (hd${DISQUE_GRUB})
###quit
###EOF

#modification de menu.list
###echo "		:: modification du menu.list"2>> /titi.txt >> /titi.txt
###cp /dossier_primaire/boot/grub/menu.lst /var/lib/installFrogLinux/menu.backup

###ancien_uuid=$(cat /var/lib/installFrogLinux/menu.backup | grep UUID | cut -d= -f3 | cut -d' ' -f1|uniq)
###set `echo $ancien_uuid`
ancien_uuid=$2
###echo "			- ancien uuid : $ancien_uuid"
###echo "			- nouveau uuid : $UUIDSLASH"

###echo "		:: modification de l'uuid et du numero du disque "
###sed "s/$ancien_uuid/$UUIDSLASH/g" </var/lib/installFrogLinux/menu.backup >/dossier_primaire/boot/grub/menu.lst 
###c###at /dossier_primaire/boot/grub/menu.lst > /var/lib/installFrogLinux/menu.backup
###s###ed "s/hd0,0/hd0,$PARTITION_GRUB/g" </var/lib/installFrogLinux/menu.backup >/dossier_primaire/boot/grub/menu.lst 
###sed "s/hd0,1/hd0,$PARTITION_GRUB/g" </var/lib/installFrogLinux/menu.backup >/dossier_primaire/boot/grub/menu.lst 
sync
sync
###cat /dossier_primaire/boot/grub/menu.lst | grep $UUIDSLASH
###cat /dossier_primaire/boot/grub/menu.lst | grep hd$DISQUE_GRUB,$PARTITION_GRUB

###rm -f menu.backup

echo "		:: demontage du disque"
if [ $(mount | grep $CHEMIN$PRIMAIRE  |wc -l |cut -d' ' -f1) -eq 1 ]
then

	if [ $(lsof -w $CHEMIN$PRIMAIRE | wc -l | cut -d' ' -f1) -gt 0 ]	
	then
		for i in $(lsof -w $CHEMIN$PRIMAIRE |grep ^[^COMMAND] |tr -s [:blank:] ' ' | cut -d' ' -f2 | uniq)
		do
			kill -9 "$i"
		done
	fi
	
	umount  -l $CHEMIN$PRIMAIRE 
	rm -rf /dossier_primaire
fi

rm -f /var/lib/installFrogLinux/menu.backup /var/lib/installFrogLinux/periph_HD.list /var/lib/installFrogLinux/num_partition.list /var/lib/installFrogLinux/periph_usb.list

printf "\n		:: Activation du Swap\n"
swapon $CHEMIN$SWAP

}

creer_partition
creer_systeme_fichier
monter_part_primaire
copie_fichier
point_de_montage

exit
