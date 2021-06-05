#!/bin/bash
if [ -f /SE_Systeme_de_gestion_de_serveurs/production/test ] 
then
scp  /SE_Systeme_de_gestion_de_serveurs/production/test /SE_Systeme_de_gestion_de_serveurs/integration/verif_prod/
elif [ -f /SE_Systeme_de_gestion_de_serveurs/backup/test ] 
then
scp  /SE_Systeme_de_gestion_de_serveurs/backup/test /SE_Systeme_de_gestion_de_serveurs/integration/verif_backup/
elif [ ! -f /SE_Systeme_de_gestion_de_serveurs/production/test ] && [ ! -f /SE_Systeme_de_gestion_de_serveurs/backup/test ] 
then
echo "aucun serveur est en  service, veillez a regler le probleme"
exit
fi

if [ -f /SE_Systeme_de_gestion_de_serveurs/integration/verif_prod/test ] 
then
echo "le serveur de production est en service"
elif [ -f /SE_Systeme_de_gestion_de_serveurs/integration/verif_backup/test ] 
then
echo "le serveur de production est hors service"
echo "le serveur de backup est en service"
else 
echo "aucun serveur est en  service, veillez a regler le probleme"
fi
