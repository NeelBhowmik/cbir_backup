#!/bin/bash

img_dataset="$1"
desc_name="$2"
dbasepath="$3"
argc="$#"

if [ ! $argc = "3" ]; then
	echo ""
	echo "USAGE: bash MergeAll.sh <img_dataset> <desc_name> <Wrokspace path>"
	echo " img_dataset    	name of the dataset of images"
	echo " desc_name      	name of the desc(hesaff+surf-mser+surf-har+surf)"
	echo " dbasepath        workspace directory path"	
	echo ""
	exit
fi

if [ "$img_dataset" = "COIL600" ]; then
	c="01-02-03-04-05-06-07-08-09-10-11-12-13-14-15-16-17-18-19-20"
elif [ "$img_dataset" = "Holiday_CV_1200_F" ]; then
	c="01"
elif [ "$img_dataset" = "Holiday_Eval_1200" ]; then
	c="01"
elif [ "$img_dataset" = "Oxford" ]; then
	c="01"
elif [ "$img_dataset" = "Oxford_Test" ]; then
	c="01"
elif [ "$img_dataset" = "Paris" ]; then
	c="01-02-03-04-05-06-07-08-09-10-11-12"
elif [ "$img_dataset" = "ParisTest" ]; then
	c="01-02-03-04-05-06-07-08-09-10-11-12"

else
	echo " Dataset unknown..."
fi
echo "Merging desc..."
dirparth=$dbasepath/2_Descriptors/data/$img_dataset
/bins/MergeDesc -dir $dirparth -desc $desc_name -class $c
