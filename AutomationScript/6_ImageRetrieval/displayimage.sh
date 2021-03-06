#!/bin/bash

if [ ! $# = "5" ]; then
	echo ""
	echo "USAGE: bash displayimage.sh <dataset> <desc> <detec> <cw> <k>"
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

datap=$(pwd)
echo "Display retrieved image:"

for folder in 0_ImgDatasets/$dataset/color/*; do
	class=$(basename $folder)
	echo " ... $class class"
	for img in $folder/*; do
		base=$(basename $img .jpg)
		qimg=0_ImgDatasets/$dataset/color/$class/$base.jpg		
		datapath=$datap/0_ImgDatasets/$dataset/color
		imgFreq=$datap/Results/$dataset/${detec}+${desc}/$class/$base/${cw}cw/Res_${base}_${desc}_${cw}w_${k}nn.rank
		imgout=$datap/Results/$dataset/${detec}+${desc}/$class/$base/${cw}cw/Res_${base}_${desc}_${cw}w_${k}nn.html
		python /bins/displayimg.pyc $datapath $qimg $imgFreq $imgout
	done
done
echo " Done!..."
