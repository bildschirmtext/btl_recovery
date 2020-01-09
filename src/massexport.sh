#!/bin/bash


gcc btl_recovery.c -o btl_recovery

for x in $*
do
	if [ -e $x ]
	then
		bn=`basename $x .BTL`
		mkdir $bn
		./btl_recovery $bn/%04d.cpt < $x > $bn/descriptions.txt
		zip -r $bn.zip $bn
	fi
done
