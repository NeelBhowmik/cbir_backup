/*
 * InvertedUniqueIndices.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: Neel
 */

/* Command to compile the code: g++ InvertedUniqueIndices.cpp -L /usr/lib -lopencv_core -lopencv_flann -o InvertedUniqueIndices -std=c++11*/

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
	string imgId;
	float val;
	if (fp.is_open()) {
		fp >> num_desc >> dims;
		dims--;//desconsidering the label
		Query.descriptors = Mat(num_desc, dims, CV_32F);
		for (int row = 0; row < num_desc; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				Query.descriptors.at<float>(row, col) = val;
			}
			fp >> imgId;
			Query.imageIds.push_back(imgId);
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

void write(vector<tr1::unordered_map<string, float> > &InvIndex, int pd,
		const char *InvertedIndexPath) {
	ofstream fp(InvertedIndexPath);
	if (fp.is_open()) {
		for (unsigned int i = 0; i < InvIndex.size(); i++) {
			for (auto it = InvIndex[i].begin(); it != InvIndex[i].end(); ++it){
				fp << it->first << " ";
				if(pd) fp << it->second << " ";
			}
			fp << endl;
		}
		fp.close();
	}
	return;
}

void associateImageIds(vector<string> &imageIds, Mat &Indices, Mat &Dists,
		int kcenters, vector<tr1::unordered_map<string, float> > &InvIndex) {
	string imageId;
	int cw;
	float dist;
	for (int i = 0; i < kcenters; i++) {
		tr1::unordered_map<string, float> hashed_imageIds;
		InvIndex.push_back(hashed_imageIds);
	}
	for (unsigned int row = 0; row < imageIds.size(); row++) {
		imageId = imageIds[row];
		cw = Indices.at<int>(row, 0);
		dist = Dists.at<int>(row, 0);
		if (!InvIndex[cw].count(imageId)) //if the key does not exists
			InvIndex[cw][imageId] = dist;
		else if (InvIndex[cw][imageId] > dist)
			InvIndex[cw][imageId] = dist;
	}
}

void computeInvIndex(const char * QueryPath, const char * CodeBookPath, int pd,
		const char * OutPath) {
	vector<tr1::unordered_map<string, float> > InvIndex;
	Descriptors Query;
	Mat CodeBook, Indices, Dists;
	readQuery(Query, QueryPath);
	readCodeBook(CodeBook, CodeBookPath);
	cv::flann::Index kdtree(CodeBook, cv::flann::KDTreeIndexParams(4));
	kdtree.knnSearch(Query.descriptors, Indices, Dists, 1,
			cv::flann::SearchParams(64));
	associateImageIds(Query.imageIds, Indices, Dists, CodeBook.rows, InvIndex);
	write(InvIndex, pd, OutPath);
}

int main(int argc, char *argv[]) {
	//Defining variables
	const char * QueryPath = NULL;
	const char * CodeBookPath = NULL;
	const char * InvertedIndexPath = NULL;
	int pd = 0;
	//USAGE message
	if (argc != 9) {
		cout << "USAGE: " << argv[0]
				<< " -q <QueryFilePath> -cb <CodeBookPath> -pd <1/0> -idx <InvertedIndexPath>"
				<< endl;
		cout
				<< "q           <file_path>    In  - query's path (each descriptor has two extra values for identifying the image and class)"
				<< endl;
		cout << "cb          <file_path>    In  - codebook's path" << endl;
		cout << "pd          <int> In  - print distance [0:no,1:yes]"
				<< endl;
		cout << "idx          <file_path>    Out - inverted index's path "
				<< endl;
		return -1;
	}
	//Parsing Options
	for (int i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-q") == 0)
			QueryPath = argv[i + 1];
		else if (strcmp(argv[i], "-cb") == 0)
			CodeBookPath = argv[i + 1];
		else if (strcmp(argv[i], "-pd") == 0)
			pd = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-idx") == 0)
			InvertedIndexPath = argv[i + 1];
		else {
			cout << "Wrong parameter" << endl;
			return -1;
		}
	}
	//Main process
	computeInvIndex(QueryPath, CodeBookPath, pd, InvertedIndexPath);
	return 0;
}
