#!/bin/usr/env bash

#USAGE: bash genLabeledDescriptors.sh

dataset="$1"
descs="$2"
argc="$#"

if [ ! $argc = "2" ]; then
	echo ""
	echo "USAGE: bash labelingDescriptorsRedis.sh <dataset> <descs>"
	echo " dataset    name of the image dataset"
	echo " descs      list of descriptors"
	echo ""
	exit
fi

echo "Building Labeled Matrix of ..."

for desc in $descs; do
	desc=${desc,,}
	rm -f data/$dataset/$desc/LD*
done

for desc in $descs; do
	desc=${desc,,}
	echo " ... ${desc^^} descriptors."
	for folder in data/$dataset/$desc/*; do
		class=$(basename $folder)
		LD=data/$dataset/$desc/LD_$class.dat
		for d_nm in $folder/*; do
			python joinIdxDescriptionsRedis.py $d_nm $LD
		done
		cat $LD >> data/$dataset/$desc/LD.dat
	done
	for LD in data/$dataset/$desc/LD*; do
		num=$(wc -l < $LD)
		dims=$(wc -w <<< $(head -n 1 < $LD))
		echo "$num $dims" > $LD.tmp
		cat $LD >> $LD.tmp
		rm -f $LD
		mv $LD.tmp $LD
	done
done

echo "Building Labeled Matrices. Done!"
