#!/bin/bash

dataset="$1"
detector="$2"

if [ ! $# = "2" ]; then
	echo ""
	echo "USAGE: bash genAllDetectorsMod.sh <dataset> <detector>"
	echo " dataset        name of the dataset of images"
	echo " detector       name of the keypoints detector [haraff|hesaff|har|heslap]"
	echo "                run \"bins_3rdParty/detect_points.ln\" to see other options"
	echo ""
	exit
fi

echo "Generating Detectors for:"
echo "...$dataset $detector"

mkdir -p 1_Detectors/data
mkdir -p 1_Detectors/data/$dataset
mkdir -p 1_Detectors/data/$dataset/$detector

for folder in 0_ImgDatasets/$dataset/pgm/*; do
	class=$(basename $folder)
	echo " ... $class class"
	mkdir -p 1_Detectors/data/$dataset/$detector/$class
	for img in $folder/*; do
		base=$(basename $img .pgm)
		ip1=0_ImgDatasets/$dataset/pgm/$class/$base.pgm
		kp_path=1_Detectors/data/$dataset/$detector/$class/$base.kp
		
		if [ "$detector" == "mser" ];  then
			/bins_3rdParty/mser.ln -t 2 -es 2 -i $ip1 -o $kp_path > $folder/temp.tmp
		else	
			/bins_3rdParty/detect_points.ln -$detector -i $ip1 -o $kp_path -thres 300 > $folder/temp.tmp
		fi	
	done
	rm -r $folder/temp.tmp
done
echo " ... $class class. Done!..."
