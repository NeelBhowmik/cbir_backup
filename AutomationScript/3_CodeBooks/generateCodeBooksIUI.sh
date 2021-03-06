#!/bin/bash

dataset="$1"
descs="$2"
kcenter="$3"

if [ ! $# = "3" ]; then
	echo ""
	echo "USAGE: bash generateCodeBooksIUI.sh <dataset> <desc_name> <kcenters>"
	echo " dataset     name of the image dataset"
	echo " descs       name of the descriptor to use (e.g.: har+sift)"
	echo " kcenters    number of code-words desired"
	echo ""
	exit
fi

echo "Computing CodeBooks for:"
echo "...$dataset $descs"

mkdir -p 3_CodeBooks/data
mkdir -p 3_CodeBooks/data/$dataset
mkdir -p 3_CodeBooks/data/$dataset/$descs
mkdir -p 4_InvertedUniqueIndices/data
mkdir -p 4_InvertedUniqueIndices/data/$dataset
mkdir -p 4_InvertedUniqueIndices/data/$dataset/$descs

Descriptor=2_Descriptors/data/$dataset/$descs/LD.dat
CodeBook=3_CodeBooks/data/$dataset/$descs/CodeBook$kcenter.dat
IUI=4_InvertedUniqueIndices/data/$dataset/$descs/InvertedUniqueIndices$kcenter.dat
kdtree=3_CodeBooks/data/$dataset/$descs/Kdtree$kcenter.dat
/bins/ComputeCodeBook -desc $Descriptor -cb $CodeBook -k $kcenter -br 2 -it 15 -l 1
/bins/InvertedUniqueIndices -q $Descriptor -cb $CodeBook -pd 0 -idx $IUI
/bins/ComputeKdtree -cb $CodeBook -o $kdtree
echo "Computation done."
