#!/bin/bash

i=0
declare -a tab
echo "Wykryte sesje:"
while read line; do
	i=$[i+1]
  echo "[$i] $line"
  tab[$i]=$line				#wczytanie pliku do tablicy linia po linii
done < ./tshark.dump

printf "Choose option from above: "
read option;
if [[ option -lt 1 || option -gt i ]]; then
	exit 1
fi


#echo "Option = $option ${tab[${option}]}"
declare -a words
i=0
for word in ${tab[${option}]}
do
	i=$[i+1]
	words[$i]=$word			#podzial wybranej linii z tablicy na slowa
   #echo $word
done
echo
for ((j=3; j<i; j++))
do 
	echo $j
	words[$j]=${words[$j]::-1}		#usuniecie srednikow z ciasteczek
	#echo ${words[$j]}
done

for ((j=3; j<=i; j++))
do
	words[$j]=$(echo ${words[$j]} | tr = \\t)			#zamiana '=' w ciasteczkach na tabulatory
	#echo ${words[$j]}
done

words[2]=${words[1]}${words[2]}					#zlepienie hosta z uri
#echo


for ((j=3; j<=i; j++))
do
	timestamp=$(date +%s)
	timestamp=$(($timestamp + 3600))
	printf "${words[1]}\tFALSE\t/\tFALSE\t$timestamp\t${words[$j]}\n" >> ./cookie.file			#dopisanie ciasteczek do pliku ciasteczek przegladarki
done
exec lynx -cfg=./cfg ${words[2]}			#uruchomienie przegladarki z konfigiem wypisanym ponizej, ktory zaladuje ciasteczka

# cat ./cfg
#SET_COOKIES:TRUE
#ACCEPT_ALL_COOKIES:TRUE
#PERSISTENT_COOKIES:TRUE
#COOKIE_FILE:./cookie.file
#COOKIE_SAVE_FILE: ./cookie.file
#FORCE_SSL_COOKIES_SECURE:FALSE

#cookie.file moze byc pusty, ale musi byc na dysku
