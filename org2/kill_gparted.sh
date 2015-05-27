#! /bin/bash 

for i in $(ps -ef | grep gparted | tr -s [:blank:] ' ' | cut -d' ' -f2)
do 
	kill -9 $i
done

exit
