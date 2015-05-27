#! /bin/bash

if [ "$#" -eq 1 ]
then
	if [ $1 = "HD" ]
	then
		declare -r CLE=$(cat /var/lib/installFrogLinux/periph_HD.list | cut -d, -f2)
		declare -r CHEMIN_PERIPH=/dev/
		prog=install_HD
		
		if [ -f  /var/lib/installFrogLinux/num_partition.list ]
		then
			PRIMAIRE=$(cat /var/lib/installFrogLinux/num_partition.list | cut -d' ' -f2)

		else
			PRIMAIRE=2
		fi

	elif [ $1 = "USB" ]
	then
		declare -r CLE=$(cat /var/lib/installFrogLinux/periph_usb.list | cut -d, -f2)
		declare -r CHEMIN_PERIPH=/dev/
		prog=instal_periph_usb
		PRIMAIRE=2
	else
		echo "erreur argument"
		
		
	fi
else
	echo "erreur argument"
	

fi



if [ $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE | wc -l | cut -d' ' -f1) -gt 0 ]	
then
	for i in $(lsof $CHEMIN_PERIPH$CLE$PRIMAIRE |grep ^[^COMMAND] |tr -s [:blank:] ' ' | cut -d' ' -f2 | uniq)
	do
			kill -9 "$i"
	done
fi

if [ $(ps -ef | grep $prog | wc -l | cut -d' ' -f1) -gt 0 ]
then	
	for i in  $(ps -ef | grep $prog |tr -s [:blank:] ' ' |cut -d' ' -f2 | uniq)
	do
		kill -9 $i
	done
fi


