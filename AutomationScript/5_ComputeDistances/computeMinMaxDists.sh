#!/bin/bash

dataset="$1"
desc="$2"
kcenters="$3"
K="$4"
classes="$5"

if [ ! $# = "5" ]; then
	echo ""
	echo "USAGE: bash computeMinMaxDists.sh <dataset> <desc> <kcenters> <k> <classes>"
	echo " dataset    name of the image dataset"
	echo " desc       descriptor to be used"
	echo " kcenters   size of codebook"
	echo " k          number of nearest neigbhors"
	echo " classes    number of nearest neigbhors"
	echo ""
	exit
fi

mkdir -p 5_ComputeDistances/data
mkdir -p 5_ComputeDistances/data/$dataset
mkdir -p 5_ComputeDistances/data/$dataset/$desc
mkdir -p 5_ComputeDistances/data/$dataset/$desc/$kcenters

CodeBook=3_CodeBooks/data/$dataset/$desc/CodeBook$kcenters.dat
Output=5_ComputeDistances/data/$dataset/$desc/$kcenters/Distances${K}nn.dat

echo "Searching mininum and maximum distances..."
for class in $classes; do
	LD=2_Descriptors/data/$dataset/$desc/LD_$class.dat
	echo " ... for $(basename $LD .dat)"
	/bins/ComputeMinMaxDist -q $LD -cb $CodeBook -k $K >> $Output
done

echo "Filtering values..."
python 5_ComputeDistances/scripts/resumeMinMaxDistances.py $Output $K

echo "Done!"
