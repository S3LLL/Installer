#!/bin/bash

   > /var/lib/installFrogLinux/disque_cle.list

for i in $(ls -d /proc/ide/ide*/hd*)
do
   if [  "$(cat   $i/media)" = disk ]
   then
      echo i=$(basename $i)
      CAPACITY=$(cat $i/capacity)
      let CAPACITY=($CAPACITY*512/1024/1024/1024)
      echo CAPACITY=$CAPACITY
      MODEL=$(cat   /proc/ide/ide0/hd*/model)
      echo Modèle=$MODEL
   fi
done

for i in $(ls -d /sys/bus/scsi/devices/[0-9]*/block/sd*)
do

   MODELE="$(cat   $i/device/model)"
   VENDEUR="$(cat   $i/../../vendor)"
   DIM="$(cat   $i/size)"
   let DIM=$DIM*512/1024/1024/1024
   echo i=$(basename $i)
   echo MODELEUSB=$VENDEUR $MODELE
   echo Dimesion=$DIM


   echo $VENDEUR $MODELE,$(basename $i),$DIM >> /var/lib/installFrogLinux/disque_cle.list

done

