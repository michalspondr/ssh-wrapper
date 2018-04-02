#!/bin/sh
# vytvori hlavickovy soubor defines.h, ktery bude obsahovat absolutni cestu
# k usr/ adresari programu (pro sezeni, roury atp.)

cesta=$(pwd);
cesta=$(echo $cesta |sed -e "s/\/src$//")	#odstrani posledni adresar
cesta=$cesta"/usr"	# pridam usr/ adresar

if [ ! -d "$cesta" ]; then	# neexistuje adresar /usr
	mkdir $cesta
	mkdir $cesta"/run" $cesta"/session"
	chmod -R 747 $cesta	# prava pro pristup weboveho serveru
fi

rm -f defines.h	#odstranim stary adresar s defines.h
echo '#define LOCALSTATEDIR '\"$cesta\" >defines.h	#vytvorim novy

