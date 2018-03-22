/*
 * ComputeMinMaxDist.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: kronox
 */

/* Command to compile the code: g++ ComputeMinMaxDist.cpp -L /usr/lib -lopencv_core -lopencv_flann -o ComputeMinMaxDist*/

#include <tr1/unordered_map>
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

//Declaring structures
struct Descriptors {
	vector<string> imageIds;
	Mat descriptors;
};

//Reading input files

void readQuery(Descriptors &Query, const char *QueryPath) {
	ifstream fp(QueryPath);
	int num_desc, dims;
	string imgId, img;
	float val;
	if (fp.is_open()) {
		fp >> num_desc >> dims;
		dims -= 1; //desconsidering the label
		Query.descriptors = Mat(num_desc, dims, CV_32F);
		for (int row = 0; row < num_desc; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				Query.descriptors.at<float>(row, col) = val;
			}
			fp >> imgId; //capturing the extra dimension
		}
		fp.close();
	}
	fp.close();
}

void readCodeBook(Mat &CodeBook, const char *CodeBookPath) {
	ifstream fp(CodeBookPath);
	int num_cw, dims;
	float val;
	if (fp.is_open()) {
		fp >> num_cw >> dims;
		CodeBook = Mat(num_cw, dims, CV_32F);
		for (int row = 0; row < num_cw; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				CodeBook.at<float>(row, col) = val;
			}
		}
		fp.close();
	}
	fp.close();
}

void computeMinMaxDistance(const char * QueryPath, const char * CodeBookPath,
		int k) {
	vector<tr1::unordered_map<string, float> > InvIndex;
	Descriptors Query;
	Mat CodeBook, Indices, Dists;
	readQuery(Query, QueryPath);
	readCodeBook(CodeBook, CodeBookPath);
	cv::flann::Index kdtree(CodeBook, cv::flann::KDTreeIndexParams(4));
	kdtree.knnSearch(Query.descriptors, Indices, Dists, k,
			cv::flann::SearchParams(64));
	float minDist = 0;
	for (int row = 0; row < Dists.rows; row++) {
		if (row == 0)
			minDist = Dists.at<float>(0, 0);
		else if (minDist > Dists.at<float>(row, 0))
			minDist = Dists.at<float>(row, 0);
	}
	vector<float> maxDists;
	for (int col = 0; col < Dists.cols; col++) {
		float maxDist = 0;
		for (int row = 0; row < Dists.rows; row++) {
			if (maxDist < Dists.at<float>(row, col))
				maxDist = Dists.at<float>(row, col);
		}
		maxDists.push_back(maxDist);
	}
	//cout << "Minimum Distance (k=" << k << "): " << minDist << endl;
	//cout << "Maximum Distance (k=" << k << "): " << maxDist << endl;
	cout << minDist << " ";
	for (unsigned int i = 0; i < maxDists.size(); i++) {
		cout << maxDists[i] << " ";
	}
	cout << endl;
}

int main(int argc, char *argv[]) {
	//Defining variables
	const char * QueryPath = NULL;
	const char * CodeBookPath = NULL;
	int k = 0;
	//USAGE message
	if (argc != 7) {
		cout << "USAGE: " << argv[0]
				<< " -q <QueryFilePath> -cb <CodeBookPath> -k <#nn>" << endl;
		cout
				<< "q           <file_path>    In  - query's path (each descriptor has two extra values for identifying the image and class)"
				<< endl;
		cout << "cb          <file_path>    In  - codebook's path" << endl;
		cout << "k           <int> In  - number of nearest neighbors" << endl;
		return -1;
	}
	//Parsing Options
	for (int i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-q") == 0)
			QueryPath = argv[i + 1];
		else if (strcmp(argv[i], "-cb") == 0)
			CodeBookPath = argv[i + 1];
		else if (strcmp(argv[i], "-k") == 0)
			k = atoi(argv[i + 1]);
		else {
			cout << "Wrong parameter" << endl;
			return -1;
		}
	}
	//Main process
	computeMinMaxDistance(QueryPath, CodeBookPath, k);
	return 0;
}
