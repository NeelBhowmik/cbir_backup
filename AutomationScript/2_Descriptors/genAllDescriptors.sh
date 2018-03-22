#!/bin/bash

dataset="$1"
desc="$2"
detector="$3"

if [ ! $# = "3" ]; then
	echo ""
	echo "USAGE: bash genAllDescriptorsMod.sh <dataset> <desc> <detector>"
	echo " dataset        name of the dataset of images"
	echo " desc           name of the descriptor [sift|surf|sc|spin]"
	echo " detector       name of the keypoints detector [haraff|hesaff|har|heslap]"
	echo "                run \"bins/compute_descriptors\" to see other options"
	echo ""
	exit
fi

echo "Generating Descriptors for:"
echo "...$dataset $desc $detector"

mkdir -p 2_Descriptors/data
mkdir -p 2_Descriptors/data/$dataset
mkdir -p 2_Descriptors/data/$dataset/$detector+${desc}
tmp=2_Descriptors/data/$dataset/temp.tmp
for folder in 0_ImgDatasets/$dataset/pgm/*; do
	class=$(basename $folder)
	echo " ... $class class"
	mkdir -p 2_Descriptors/data/$dataset/$detector+${desc}/$class
	
	for img in $folder/*; do
		base=$(basename $img .pgm)

		image=0_ImgDatasets/$dataset/pgm/$class/$base.pgm
		keypoints=1_Detectors/data/$dataset/$detector/$class/$base.kp
		description=2_Descriptors/data/$dataset/$detector+${desc}/$class/$base.desc

		if [ "$desc" == "surf" ];  then
			/bins_3rdParty/surf -i $image -o $description -p1 $keypoints -nl -q
		else	
			/bins_3rdParty/compute_descriptors.ln -$desc -i $image -p1 $keypoints -o1 $description > $tmp
		fi	  
	python 2_Descriptors/scripts/formatDescriptor.py $description $desc $detector  
	done
	if [ ! "$desc" == "surf" ];  then
		rm -r $tmp
	fi
done
echo " ... $class class. Done!..."
