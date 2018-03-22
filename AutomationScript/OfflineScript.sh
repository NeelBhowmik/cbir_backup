#!/bin/bash
#Make sure you are in the main path (e.g.:Workspace)

if [ ! $# = "9" ]; then
	echo ""
	echo "USAGE: bash genAllDetectorsMod.sh <dataset> <detector>"
	echo " dataset        name of the dataset of images"
	echo " detect1        name of the keypoints detector1"
	echo " detect2        name of the keypoints detector2"
	echo " detect3        name of the keypoints detector3"
	echo " desc1          name of the descriptor1"
	echo " desc2          name of the descriptor2"
	echo " desc3          name of the descriptor3"
	echo " cw             Codebook size"
	echo " dbasepath      Directory path"
	echo ""
	exit
fi

dataset="$1"
detect1="$2"
detect2="$3"
detect3="$4"
desc1="$5"
desc2="$6"
desc3="$7"
cw="$8"
dbasepath="$9"

#bash 1_Detectors/genAllDetectors.sh $dataset $detect1
#bash 1_Detectors/genAllDetectors.sh $dataset $detect2
#bash 1_Detectors/genAllDetectors.sh $dataset $detect3
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc1 $detect1
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc2 $detect1
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc3 $detect1
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc1 $detect2
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc2 $detect2
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc3 $detect2
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc1 $detect3
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc2 $detect3
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc3 $detect3
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc1 $detect1

#bash 2_Descriptors/MergeAll.sh $dataset $detect1+$desc1-$detect2+$desc1-$detect3+$desc1 $dbasepath 
#bash 2_Descriptors/MergeAll.sh $dataset $detect1+$desc2-$detect2+$desc2-$detect3+$desc2 $dbasepath
#bash 2_Descriptors/MergeAll.sh $dataset $detect1+$desc3-$detect2+$desc3-$detect3+$desc3 $dbasepath
 
#bash 1_Descriptors/labelDescriptions.sh <name_image_dataset> <list_of_descriptors>
#bash 2_Descriptors/labelDescriptions.sh $dataset $detect1+$desc1+$detect2+$desc1+$detect3+$desc1
#bash 2_Descriptors/labelDescriptions.sh $dataset $detect1+$desc2+$detect2+$desc2+$detect3+$desc2
#bash 2_Descriptors/labelDescriptions.sh $dataset $detect1+$desc3+$detect2+$desc3+$detect3+$desc3

#bash 3_CodeBooks/generateCodeBooksIUI.sh <dataset> <desc_name> <kcenters>
bash 3_CodeBooks/generateCodeBooksIUI.sh $dataset $detect1+$desc1+$detect2+$desc1+$detect3+$desc1 $cw
#bash 3_CodeBooks/generateCodeBooksIUI.sh $dataset $detect1+$desc2+$detect2+$desc2+$detect3+$desc2 $cw
#bash 3_CodeBooks/generateCodeBooksIUI.sh $dataset $detect1+$desc3+$detect2+$desc3+$detect3+$desc3 $cw
