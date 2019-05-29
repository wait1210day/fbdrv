#!/usr/bin/env bash

platfrom=$(uname -s)

if [[ ! -d ./tools/platfrom/${platfrom} ]]; then
	echo "Sorry, Your platfrom wasn't supported on this version. Please update the source code"
	echo "If the source code is the latest version, that means you can't use this program"
	exit 1
fi

cd ./tools/platfrom/${platfrom}

gcc -g -lm -Wall -Wextra -O2 -o painter painter.c
gcc -g -lm -Wall -Wextra -O2 -o getprop getprop.c

echo "Compiltion was OK. Would you like to run testing-program now ? [y/n]"
read yn

if [[ "x$yn" = "xy" ]]; then
	./getprop | more
	echo "Next test will start at 30sec later..."
	sleep 30s
	./painter
else
	echo "Exited with code 0"
	exit 0
fi
