/*
 * ComputeCodebook
 *
 *  Created on: Nov 10, 2014
 *      Author: Neel
 */ 
// g++ ComputeCodebook.cpp -L /usr/lib -lopencv_core -lopencv_flann -o ComputeCodeBook

//#include <flann/flann.hpp>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
//#include <opencv/cv.h>
#include <opencv2/flann.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

using namespace cv;
using namespace std;

//http://linux.die.net/man/2/getrusage
inline double getUserTime(struct rusage *p) {
	if (getrusage(RUSAGE_SELF, p) != 0)
		cout << "Error while obtaining resources." << endl;	//FatalError
	return p->ru_utime.tv_sec + p->ru_utime.tv_usec / 1000000.0f;
}

int main(int argc, char** argv) {
	//Defining variables
	const char *CodeBookPath = NULL;
	const char *DescriptorsPath = NULL;
	int kcenters = 1;
	int branches = 25;
	int iterations = 15;
	int i, descs, dims, l=1;
	string label;
	//USAGE message
	if (argc != 13) {
		cout
				<< "USAGE: ./ComputeCodebooks -desc <Descriptors> -cb <CodeBook> -k <kcenters> -br <branches> -it <iterations> -l <0|1>"
				<< endl;
		cout
				<< "desc                      <file_path>    In  - path of the desired Descriptors file.     "
				<< endl;
		cout
				<< "cb                        <file_path>    Out - path of the desired CodeBook file.        "
				<< endl;
		cout
				<< "k                         <int>          In  - Number of desired codewords.              "
				<< endl;
		cout
				<< "br                        <int>          In  - Number of desired branches.               "
				<< endl;
		cout
				<< "it                        <int>          In  - Number of desired iterations.             "
				<< endl;
		cout
				<< "l                         <int>          In  - 0: if descriptors are not labeled, 1: if descriptors are labeled. "
				<< endl;
		return -1;
	}
	//Parsing Options
	for (i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-desc") == 0)
			DescriptorsPath = argv[i + 1];
		else if (strcmp(argv[i], "-cb") == 0)
			CodeBookPath = argv[i + 1];
		else if (strcmp(argv[i], "-k") == 0)
			kcenters = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-br") == 0)
			branches = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-it") == 0)
			iterations = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-l") == 0)
			l = atoi(argv[i + 1]);
		else {
			cout << "Wrong parameter" << endl;
			return -1;
		}
	}
	//Main process
	float *Descriptors;
	/**********************************Read Descriptors**********************************/
	ifstream desc_file(DescriptorsPath);
	if (desc_file.is_open()) {
		desc_file >> descs >> dims;
		dims = l ? dims-1 : dims; //disconsider one dimension if the matrix is labeled
		Descriptors = new float[descs * dims];
		for (int i = 0; i < descs; i++) {
			for (int j = 0; j < dims; j++)
				desc_file >> Descriptors[i * dims + j];
			if (l) desc_file >> label;
		}
		desc_file.close();
	} else
		cout << "Unable to open file";
	/******************************Apply Hierarchichal Kmeans****************************/
	cvflann::Matrix<float> DescMatrix(Descriptors, descs, dims);

	float *CodeWords = new float[kcenters * dims];
	cvflann::Matrix<float> CodeWordsMatrix(CodeWords, kcenters, dims);

	cvflann::KMeansIndexParams Params = cvflann::KMeansIndexParams(branches,
			iterations);

	struct rusage usage;
	double time = getUserTime(&usage);
	kcenters = cvflann::hierarchicalClustering<cvflann::L2<float> >(DescMatrix,
			CodeWordsMatrix, Params);
	time = getUserTime(&usage) - time;
	cout << "Time: " << time << endl;
	cout << "K-Centers: " << kcenters << endl;
	/**********************************Write CodeBook***********************************/
	ofstream CB_file;
	CB_file.open(CodeBookPath);
	CB_file << kcenters << " " << dims << endl;
	for (int i = 0; i < kcenters; i++) {
		for (int j = 0; j < dims; j++)
			CB_file << " " << CodeWords[i * dims + j];
		CB_file << endl;
	}
	CB_file.close();
	//Freeing up space
	delete[] Descriptors;
	delete[] CodeWords;
}
