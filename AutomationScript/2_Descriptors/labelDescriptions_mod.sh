#!/bin/bash

#USAGE: bash genLabeledDescriptors.sh

dataset="$1"
descs="$2"
argc="$#"

if [ ! $argc = "2" ]; then
	echo ""
	echo "USAGE: bash labelingDescriptors.sh <dataset> <descs>"
	echo " dataset    name of the image dataset"
	echo " descs      list of descriptors"
	echo ""
	exit
fi

echo "Building Labeled Matrix of ..."

for desc in $descs; do
	desc=${desc,,}
	rm -f 2_Descriptors/data/$dataset/$desc/LD*
done

for desc in $descs; do
	desc=${desc,,}
	echo " ... ${desc^^} descriptors."
	for folder in 2_Descriptors/data/$dataset/$desc/*; do
		class=$(basename $folder)
		LD=/tmp/LD_$class.dat
		for d_nm in $folder/*; do
			python 2_Descriptors/scripts/joinLabelDescriptions.py $d_nm $LD
		done
		cat $LD >> tmp/LD.dat
		cp $LD 2_Descriptors/data/$dataset/$desc/LD_$class.dat 
	done
	
	for LD in 2_Descriptors/data/$dataset/$desc/LD*; do
		cp $LD /tmp/LD.tmp
		num=$(wc -l < $LD)
		dims=$(wc -w <<< $(head -n 1 < $LD))
		echo "$num $dims" > /tmp/LD.tmp
		cat $LD >> /tmp/LD.tmp
		rm -f $LD
		mv /tmp/LD.tmp $LD
	done
done


echo "Building Labeled Matrices. Done!"
