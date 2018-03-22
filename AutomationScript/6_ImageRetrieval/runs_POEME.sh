#!/bin/bash

if [ ! $# = "5" ]; then
	echo ""
	echo "USAGE: bash runs_POEME.sh <dataset> <desc> <detec> <cw> <k> <verb>"
	echo " dataset    name of the image dataset"
	echo " desc       list of descriptors to use (e.g.: this way -> sift-surf)"
	echo " detec      list of detectors to use (e.g.: this way -> hesaff-mser)"
	echo " cw         list of sizes of the codebooks to use (e.g.: 500000-500000)"
	echo " k          numbers of nearest neigbhors"
	echo ""
	exit
fi

dataset="$1"
desc="$2"
detec="$3"
cw="$4"
k="$5"


if [ "$dataset" = "COIL600" ]; then
	t="600"
	c="01-02-03-04-05-06-07-08-09-10-11-12-13-14-15-16-17-18-19-20"
	#c="01"
elif [ "$dataset" = "essai" ]; then
	t="100"
	c="class_00-class_01-class_02-class_03-class_04-class_05-class_06-class_07-class_08"
else
	echo " Dataset unknown..."
fi

/bins/RetrieveImages_POEME -dir $(pwd) -dat $dataset -det $detec -desc $desc -cw $cw -k $k -t $t -c $c -verb 1
