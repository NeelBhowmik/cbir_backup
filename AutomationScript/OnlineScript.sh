#!/bin/usr/env bash
#Make sure you are in the main path (e.g.:Workspace)
detec1=hesaff

desc1=sift
desc2=sc
desc3=surf

dataset=essai
kcenter=20000
knn=2

# For the query images: Lets assume, the query images are stored in different folder than training images. The training image dataset is - COIL600, and the query images are in COIL600_Test. Then we much copy CodeBook and Inverted Index files from COIL600 to COIL600_Test. In this scenario, we must execute below commands: 
# cp -R 3_CodeBooks/data/COIL600/ 3_CodeBooks/data/COIL600_Test
# cp -R 4_InvertedUniqueIndices/data/COIL600/ 4_InvertedUniqueIndices/data/COIL600_Test

#bash genAllDetectors.sh <dataset> <detector>
#bash 1_Detectors/genAllDetectors.sh $dataset $detect1

#bash genAllDescriptors.sh <dataset> <desc> <detector>
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc1 $detec1
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc2 $detec1
#bash 2_Descriptors/genAllDescriptors.sh $dataset $desc3 $detec1

#bash 1_Descriptors/labelDescriptions.sh <name_image_dataset> <list_of_descriptors>
#bash 2_Descriptors/labelDescriptions.sh $dataset $detec1+$desc1
#bash 2_Descriptors/labelDescriptions.sh $dataset $detec1+$desc2
#bash 2_Descriptors/labelDescriptions.sh $dataset $detec1+$desc3

#bash computeMinMaxDists.sh <dataset> <desc> <kcenters> <k> <classes>
bash 5_ComputeDistances/computeMinMaxDists.sh $dataset $detec1+$desc1 $kcenter $knn "class_00 class_01 class_02 class_03 class_04 class_05 class_06 class_07 class_08"
bash 5_ComputeDistances/computeMinMaxDists.sh $dataset $detec1+$desc2 $kcenter $knn "class_00 class_01 class_02 class_03 class_04 class_05 class_06 class_07 class_08"
#bash 5_ComputeDistances/computeMinMaxDists.sh $dataset $detec1+$desc3 $kcenter $knn "class_00 class_01 class_02 class_03 class_04 class_05 class_06 class_07 class_08"

#bash runs.sh <dataset> <desc> <detec> <cw> <k>
#bash 6_ImageRetrieval/runs_POEME.sh $dataset $desc1 $detec1 $kcenter $knn
bash 6_ImageRetrieval/runs_POEME.sh $dataset $desc1-$desc2 $detec1 $kcenter-$kcenter $knn
bash 6_ImageRetrieval/displayimage.sh $dataset $desc1-$desc2 $detec1 $kcenter-$kcenter $knn
