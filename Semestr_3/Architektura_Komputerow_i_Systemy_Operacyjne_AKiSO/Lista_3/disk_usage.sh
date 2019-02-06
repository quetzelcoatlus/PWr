#!/bin/bash

function draw {
cat br.txt | tail -n60 > br60.txt
cat bw.txt | tail -n60 > bw60.txt
cat cpu.txt | tail -n60 > cpu60.txt

declare -a BRD
declare -a BWD
declare -a CPU

######################################### WCZYTANIE #######################################

exec 10<&0

########## READ ###############

exec < br60.txt
#exec < br.txt | tail -n60
local count=0

while read LINE; do

    BRD[$count]=$LINE
    ((count++))
done

########## WRITE ###############

exec < bw60.txt
#exec < bw.txt | tail -n60

local count=0

while read LINE; do

    BWD[$count]=$LINE
    ((count++))
done

########## CPU ###############

exec < cpu60.txt

local count=0

while read LINE; do

    CPU[$count]=$LINE
    ((count++))
done

exec <&10 10<&-


######################################### READ #######################################

local maxbrd=${BRD[0]}
for (( i=0;i<${#BRD[@]};i++ )); do
	if [[ ${BRD[${i}]} -gt maxbrd ]]; then
		maxbrd=${BRD[${i}]}
	fi
done

declare -a r

r[1]=$[5*maxbrd/5]
r[2]=$[4*maxbrd/5]
r[3]=$[3*maxbrd/5]
r[4]=$[2*maxbrd/5]
r[5]=$[1*maxbrd/5]


declare -a rd

local d=0;
local t=maxbrd
local r="B/s"

if [[ t -gt 1024 ]]; then
	t=$[t/1024]
	r="KB/s"
	d=1
	if [[ t -gt 1024 ]]; then
		t=$[t/1024]
		r="MB/s"
		d=2
	fi
fi

local power=1
for (( i=0; i<$d; i++ )); do
	power=$[power*1024]
done

for (( i=1; i<6; i++ )); do
	rd[${i}]=$(echo "scale=2; ${r[${i}]}/$power" | bc)
	if [ "${rd[${i}]:0:1}" == "." ]; then 
	 	rd[${i}]="0"${rd[${i}]}
	fi
	rd[${i}]=${rd[${i}]}" $r"
done


local big="                                \033[38;5;44mREAD\n"



for (( i=1; i<6; i++ )); do
	big=$big$'\033[38;5;2m|\033[38;5;3m'
	
	for (( j=0; j<60; j++)); do
		if [[ ${BRD[${j}]} -ge ${r[${i}]} ]]; then
			big=$big$'\033[38;5;1m#\033[38;5;3m'
		else
			big=$big"."
		fi
	done
	big=$big$'\033[38;5;2m|\033[38;5;4m '${rd[${i}]}$'\033[38;5;3m\033[K\n'
done

big=$big"\033[38;5;8m 60   55   50   45   40   35   30   25   20   15   10   5    0\n"

######################################### WRITE #######################################

local maxbwd=${BWD[0]}
for (( i=0;i<${#BWD[@]};i++ )); do
	if [[ ${BWD[${i}]} -gt maxbwd ]]; then
		maxbwd=${BWD[${i}]}
	fi
done

declare -a w

w[1]=$[5*maxbwd/5]
w[2]=$[4*maxbwd/5]
w[3]=$[3*maxbwd/5]
w[4]=$[2*maxbwd/5]
w[5]=$[1*maxbwd/5]

declare -a wd

local d=0;
local t=maxbwd
local r="B/s"

if [[ t -gt 1024 ]]; then
	t=$[t/1024]
	r="KB/s"
	d=1
	if [[ t -gt 1024 ]]; then
		t=$[t/1024]
		r="MB/s"
		d=2
	fi
fi

local power=1
for (( i=0; i<$d; i++ )); do
	power=$[power*1024]
done

for (( i=1; i<6; i++ )); do
	wd[${i}]=$(echo "scale=2; ${w[${i}]}/$power" | bc)
	if [ "${wd[${i}]:0:1}" == "." ]; then 
	 	wd[${i}]="0"${wd[${i}]}
	fi
	wd[${i}]=${wd[${i}]}" $r"
done


local big2="                               \033[38;5;44mWRITE\n"


for (( i=1; i<6; i++ )); do
	big2=$big2$'\033[38;5;2m|\033[38;5;3m'
	
	for (( j=0; j<60; j++)); do
		if [[ ${BWD[${j}]} -ge ${w[${i}]} ]]; then
			big2=$big2$'\033[38;5;1m#\033[38;5;3m'
		else
			big2=$big2"."
		fi
	done
	big2=$big2$'\033[38;5;2m|\033[38;5;4m '${wd[${i}]}$'\033[38;5;3m\033[K\n'
done

big2=$big2"\033[38;5;8m 60   55   50   45   40   35   30   25   20   15   10   5    0\n"

######################################### CPU #######################################

local maxcpu=${CPU[0]}
for (( i=0;i<${#CPU[@]};i++ )); do
	if [[ ${CPU[${i}]} -gt maxcpu ]]; then
		maxcpu=${CPU[${i}]}
	fi
done

declare -a c

c[1]=$[5*maxcpu/5]
c[2]=$[4*maxcpu/5]
c[3]=$[3*maxcpu/5]
c[4]=$[2*maxcpu/5]
c[5]=$[1*maxcpu/5]


declare -a cd

cd[1]=$(echo "scale=2; 5*$maxcpu/500" | bc)
cd[2]=$(echo "scale=2; 4*$maxcpu/500" | bc)
cd[3]=$(echo "scale=2; 3*$maxcpu/500" | bc)
cd[4]=$(echo "scale=2; 2*$maxcpu/500" | bc)
cd[5]=$(echo "scale=2; 1*$maxcpu/500" | bc)

for (( i=1; i<6; i++)); do
	if [ "${cd[${i}]:0:1}" == "." ]; then 
	 	cd[${i}]="0"${cd[${i}]}
	fi
done

local big3="                              \033[38;5;44mCPU LOAD\n"


for (( i=1; i<6; i++ )); do
	big3=$big3$'\033[38;5;2m|\033[38;5;3m'
	
	for (( j=0; j<60; j++)); do
		if [[ ${CPU[${j}]} -ge ${c[${i}]} ]]; then
			big3=$big3$'\033[38;5;1m#\033[38;5;3m'
		else
			big3=$big3"."
		fi
	done
	big3=$big3$'\033[38;5;2m|\033[38;5;4m '${cd[${i}]}$'\033[38;5;3m\033[K\n'
done

big3=$big3"\033[38;5;8m 60   55   50   45   40   35   30   25   20   15   10   5    0"


big=$'\033[3;0H'$big$'\n'$big2$'\n'$big3
printf "$big"

}

################################################ BEGINNING #########################################

clear
tput civis 

name=$(echo `cat /proc/diskstats | grep -e 'sda ' | awk '{print $3}'` ) 

sr=$(echo `cat /proc/diskstats | grep -e 'sda ' | awk '{print $6}'`)
let br=$[sr*512]

sw=$(echo `cat /proc/diskstats | grep -e 'sda ' | awk '{print $10}'`)
let bw=$[sw*512]

zero=0
echo $zero > br.txt
echo $zero > bw.txt
echo $zero > cpu.txt

for (( i=0; i<58; i++)); do
	echo $zero >> br.txt
	echo $zero >> bw.txt
	echo $zero >> cpu.txt
done


while true; do
	sr2=$(echo `cat /proc/diskstats | grep -e 'sda ' | awk '{print $6}'`)
	br2=$[sr2*512]

	sw2=$(echo `cat /proc/diskstats | grep -e 'sda ' | awk '{print $10}'`)
	bw2=$[sw2 * 512]

	cpu2=$(echo `cat /proc/loadavg | awk '{print $1}' ` )

	brd=$[br2-br]
	bwd=$[bw2-bw]

	echo $brd >> br.txt
	echo $bwd >> bw.txt
	echo $(echo "($cpu2*100)/1" |bc) >> cpu.txt

	r="B/s"
	if [[ brd -gt 1024 ]]; then
		brd=$[brd/1024]
		r="KB/s"
		if [[ brd -gt 1024 ]]; then
			brd=$[brd/1024]
			r="MB/s"
		fi
	fi

	w="B/s"
	if [[ bwd -gt 1024 ]]; then
		bwd=$[bwd/1024]
		w="KB/s"
		if [[ bwd -gt 1024 ]]; then
			bwd=$[bwd/1024]
			w="MB/s"
		fi
	fi

	printf "\033[0;0H\033[38;5;48mRead: $brd $r\033[K"
	printf "\033[0;32H\033[38;5;48mWrite: $bwd $w\033[K"
	printf "\033[0;64H\033[38;5;48mCpu load: $cpu2\033[K"
	
	br=$br2
	bw=$bw2
	draw
	sleep 1
done
