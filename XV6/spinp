#!/bin/sh

if [ $# != 1 ] || [ ! -f "$1" ]; then
	echo 'usage: spinp file.p' 1>&2
	exit 1
fi

rm -f $1.trail
spin -a $1 || exit 1
cc -DSAFETY -DREACH -DMEMLIM=500 -o pan pan.c
pan -i
rm pan.* pan
if [ -f $1.trail ]; then
	spin -t -p $1
fi

