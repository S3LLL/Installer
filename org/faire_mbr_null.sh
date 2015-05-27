#! /bin/bash
 
	> /var/lib/installFrogLinux/mbr_null
i=1

while [ $i -le 5120 ]
do
	echo -n 0 >> /var/lib/installFrogLinux/mbr_null
	((i++))
done


