#!/bin/bash

dataset="$1"
ext="$2"

if [ ! $# = "2" ]; then
	echo ""
	echo "USAGE: bash genDetectors.sh <dataset> <Image extension>"
	echo " dataset        name of the dataset of images"
	echo " ext            Desired Image extention [pgm, png]"
	echo ""
	exit
fi

echo "Converting images:"
echo $dataset

mkdir -p 0_ImgDatasets/$dataset/$ext

for folder in 0_ImgDatasets/$dataset/color/*; do
	class=$(basename $folder)
	mkdir -p 0_ImgDatasets/$dataset/$ext/$class
	for img in $folder/*; do
		base=$(basename $img .jpg)	
		ipImp=0_ImgDatasets/$dataset/color/$class/$base.jpg
		opImg=0_ImgDatasets/$dataset/$ext/$class/$base
		bins/ImageConvert -i $ipImp -o $opImg -ext $ext
	done	
done
echo "Done!"
