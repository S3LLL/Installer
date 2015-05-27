#! /bin/bash

################################################################################
#Florent Pavageau
#20/04/2008
#
#Ce script prepare la cle usb et installe froglinux dessus
################################################################################


declare -r TAILLE_MIN=8000.0
declare -r CHEMIN_PERIPH=/dev/
###declare -r label=FrogUsb
#####declare -r point_montage=/media/"$label"

###umount /media/FrogUsb
umount /t

mkdir /t    # le point de montage doit être à la racine principale: bug de grub
	
declare -r point_montage=/t
            # le media sera monté une fois le disque grub connu, plus bas

set -x
touch /var/lib/installFrogLinux/fichiers_exclus_utilisateur.list

./var/lib/installFrogLinux/detect_disque_cle        # les résultats sont dans disque_cle.list

# si le contenu de disque_cle.list est vide, il n'y a aucun périphérique d'installation

if [ ! -s /var/lib/installFrogLinux/disque_cle.list ]        # les résultats sont dans disque_cle.list 
   then
     echo "Il n'y a aucun périphérique de disponible pour installer FrogLinux"
     exit 1
fi


# On doit afficher ici la liste des disques ou cle et demander de choisir
# Attention, c'est l'installer graphique qui doit demander de choisir





#on recupere la taille et son unite de la cle choisie
taille_cle=`cat /var/lib/installFrogLinux/periph_usb.list | cut -d' ' -f1 | cut -d, -f3`
unite_taille=`cat /var/lib/installFrogLinux/periph_usb.list | cut -d' ' -f2`

#echo $taille_cle


# on converti en Mo la taille de la cle si necessaire
case "$unite_taille" in
	"Go"|"GB")
		taille_cle=$(echo "$taille_cle*1000.0" | bc);;
	"Ko"|"KB")
		taille_cle=$(echo "$taille_cle/1000.0" | bc);;
esac
#echo $taille_cle


# on teste si la taille de la cle est suffisante (>=8 Go)
if [ $(echo "$taille_cle>=$TAILLE_MIN" | bc) -eq 1 ]
then
	echo "taille ok"
else
	echo "taille insuffisante"
	exit 2
fi


#############################################################
#	            PARTITIONS & SWAP
#############################################################

rm /var/lib/installFrogLinux/mbr_null 2>/dev/null
. /var/lib/installFrogLinux/faire_mbr_null.sh
echo -n "effacement des partitions ..."
#Effacer partitions
#########dd if=mbr_null of=$CHEMIN_PERIPH$CLE #2>/dev/null >/dev/null
echo "ok"
rm /var/lib/installFrogLinux/mbr_null


echo -n "creation des partitions ..."


parted -s $CHEMIN_PERIPH$CLE rm 1
parted -s $CHEMIN_PERIPH$CLE rm 2
parted -s $CHEMIN_PERIPH$CLE rm 3
parted -s $CHEMIN_PERIPH$CLE rm 4

#creations partitions
SWAP=1
PRIMAIRE=2

# creation de la partition swap 
parted -s $CHEMIN_PERIPH$CLE mkpart primary 10MB 500MB

# Elle devient de type swap

parted -s $CHEMIN_PERIPH$CLE set 1  d on

# creation de la partition utile 

parted -s $CHEMIN_PERIPH$CLE mkpart primary 500MB 100%

parted $CHEMIN_PERIPH$CLE set 2  b on

if [ $? -ne 0 ]
then
	echo "erreur lors de la creation des partitions"
	#exit 2
fi


sleep 5 # nécessaire pour accorder un temps de réponse au disque


echo -n "creation ext3 sur la partition primaire ..."

# on supprime les processus eventuels pour pouvoir demonter la cle
if [ $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE | wc -l | cut -d' ' -f1) -gt 0 ]	
then
	for i in $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE |grep ^[^COMMAND] |tr -s [:blank:] ' ' | cut -d' ' -f2 | uniq)
	do
			kill -9 "$i"
	done
fi

# demonte la cle si besoin
if [ $(mount | grep $CHEMIN_PERIPH$CLE$PRIMAIRE  |wc -l |cut -d' ' -f1) -eq 1 ]
then
	
	umount  -l $CHEMIN_PERIPH$CLE$PRIMAIRE  
fi


#creation des systemes de fichiers
mkfs.ext3 $CHEMIN_PERIPH$CLE$PRIMAIRE -L "$label" #2>/dev/null >/dev/null

echo -n "creation du swap ..."
mkswap $CHEMIN_PERIPH$CLE$SWAP #>/dev/null

#echo -n "activation du swap ..."
#swapon $CHEMIN_PERIPH$CLE$SWAP #>/dev/null
#echo "ok"


rm -rf $point_montage/*
mkdir $point_montage 2>/dev/null

mount  $CHEMIN_PERIPH$CLE$PRIMAIRE $point_montage

#echo $point_montage

#############################################################
#	    	 COPIE DU SYSTEME
#############################################################

lien_dvd_live="/bin /boot /lib /opt /sbin /usr"
:>fichiers_exclus_dvd_live.list
echo " remplacement des liens symboliques par les fichiers ciblés"
for i in $lien_dvd_live
do
	if [ $(ls -ld $i | grep ^l |wc -l) -eq 1 ]
	then
		echo "$i" >> 	fichiers_exclus_dvd_live.list
		mkdir $point_montage/$i
		fichier_pointe=$(readlink -f $i)
		rsync -r -o -g -l -p -E $fichier_pointe/* $point_montage/$i/
	fi

done
cat fichiers_exclus_dvd_live.list


echo -n "copie de Froglinux sur le peripherique..."
rsync -r -o -g -l -p -E --exclude-from=/var/lib/installFrogLinux/fichiers_exclus_base.list --exclude-from=/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list --exclude-from=fichiers_exclus_dvd_live.list / $point_montage

if [ $? -ne 0 ]
then
	echo "erreur lors de la copie du systeme"
	#exit 2
fi

echo "fin copie"


#creer fichiers exclus
for i in $(cat /var/lib/installFrogLinux/fichiers_exclus_base.list | grep [^/dossier_primaire]|grep [^/etc/froglinux/DVDENCOURS])
do
	mkdir $point_montage"$i"
done


#donner les bons droits a /tmp
chmod 777 $point_montage/tmp
chmod a+t $point_montage/tmp


#creer l'arborescence vide de /var/log et fichier mtab
find /var/log -type d -exec mkdir $point_montage/{} \; 
:>$point_montage/etc/mtab


#creation du fstab
UUIDPRIMAIRE=$(vol_id $CHEMIN_PERIPH$CLE$PRIMAIRE | grep ^ID_FS_UUID= |cut -d= -f2)
UUIDSWAP=$(vol_id $CHEMIN_PERIPH$CLE$SWAP | grep ^ID_FS_UUID= |cut -d= -f2)
echo $UUIDPRIMAIRE
echo $UUIDSWAP

cat > $point_montage/etc/fstab <<EOF
# /etc/fstab:  $label
#
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
proc            /proc           proc    defaults        0       0
#/dev/sda2
UUID=$UUIDPRIMAIRE	/	ext3	defaults,errors=remount-ro	0	1
#/dev/sda1
UUID=$UUIDSWAP	none	swap	sw	0	0
EOF

cp $point_montage/etc/fstab $point_montage/etc/fstab.clean

#############################################################
#	                   GRUB
#############################################################

#on recupere le numero du disque et de la partition
DISQUE_GRUB=$(sfdisk -s | grep ^/dev |grep -n $CHEMIN_PERIPH$CLE |cut -d: -f1)
DISQUE_GRUB=$(expr $DISQUE_GRUB - 1)
PARTITION_GRUB=$(expr $PRIMAIRE - 1)
echo "numero & partition : $DISQUE_GRUB , $PARTITION_GRUB"



mkdir $point_montage/proc
mkdir $point_montage/dev


mount -t proc none $point_montage/proc

mount -o bind /dev $point_montage/dev



grub-install --no-floppy --recheck --root-directory=$point_montage $CHEMIN_PERIPH$CLE

set +x
####cp -r /dev/* $point_montage/dev

####cp /proc/devices $point_montage/proc
set -x

chroot $point_montage grub-mkconfig -o /boot/grub/grub.cfg


###grub-install --recheck --no-floppy --root-directory=$point_montage $CHEMIN_PERIPH$CLE$PRIMAIRE #2> /dev/null > /dev/null
###grub-install --recheck --no-floppy --root-directory=$point_montage $CHEMIN_PERIPH$CLE$PRIMAIRE 
###update-grub -y 2> /dev/null #> /dev/null
###update-grub -y 

###grub  --batch <<EOF #>/dev/null
###root (hd${DISQUE_GRUB},$PARTITION_GRUB)
###setup (hd${DISQUE_GRUB})
###quit
###EOF

#modification de menu.list
###cp $point_montage/boot/grub/menu.lst /var/lib/installFrogLinux/menu.backup

###ancien_uuid=$(cat /var/lib/installFrogLinux/menu.backup | grep UUID | cut -d= -f3 | cut -d' ' -f1|uniq)
###set `echo $ancien_uuid`
###ancien_uuid=$2
###echo $ancien_uuid

###sed "s/$ancien_uuid/$UUIDPRIMAIRE/g" </var/lib/installFrogLinux/menu.backup >$point_montage/boot/grub/menu.lst 
###cat $point_montage/boot/grub/menu.lst > /var/lib/installFrogLinux/menu.backup
###sed "s/hd0,0/hd0,1/g" </var/lib/installFrogLinux/menu.backup >$point_montage/boot/grub/menu.lst 

###rm -f /var/lib/installFrogLinux/menu.backup

# on supprime les processus eventuels pour pouvoir demonter la cle
	if [ $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE | wc -l | cut -d' ' -f1) -gt 0 ]	
	then
		for i in $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE |tr -s [:blank:] ' ' | cut -d' ' -f2 | uniq)
		do
			kill -9 $i
		done
	fi


umount $point_montage/proc
umount $point_montage/dev
umount $point_montage

umount  $CHEMIN_PERIPH$CLE$PRIMAIRE 
#####rm -rf $point_montage
exit 