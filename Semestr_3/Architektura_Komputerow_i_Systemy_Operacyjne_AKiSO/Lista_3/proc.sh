#!/bin/bash

for i in $( ls /proc/ | grep -E '^[0-9]+$' ); do
	if [ -e /proc/$i/stat ]; then
		name=$(sudo cat /proc/$i/stat | awk '{print $2}')
		pid=$(sudo cat /proc/$i/stat | awk '{print $1}')
		ppid=$(sudo cat /proc/$i/stat | awk '{print $4}')		
		fd=$(sudo ls /proc/$i/fd/ | wc -l)
	
		printf "\033[38;5;48mName:$name  \033[38;5;2mPID:$pid  \033[38;5;4mPPID:$ppid  \033[38;5;1mFILES OPEN:$fd\n"
	fi
done
