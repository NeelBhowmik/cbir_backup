/*
 * ComputeKdtree.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: Neel
 */


/* Command to compile the code: g++ ComputeKdtree.cpp -L /usr/lib -lopencv_core -lopencv_flann -o ComputeKdtree*/

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

void readCodeBook(Mat &CodeBook, string CodeBookPath) {
	ifstream fp(CodeBookPath.c_str());
	int dims, num_cw;
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

void buildKdtree(string CodeBookPath, string OutputFilePath) {
	Mat CodeBook;
	readCodeBook(CodeBook, CodeBookPath);
	cv::flann::Index kdtree(CodeBook, cv::flann::KDTreeIndexParams(4));
	kdtree.save(OutputFilePath);
}

int main(int argc, char *argv[]) {
	//Defining variables
	string CodeBookPath;
	string OutputFilePath;
	//USAGE message
	if (argc != 5) {
		cout << "USAGE: " << argv[0]
				<< " -cb <CodeBookPath> -o <OutputFilePath>"
				<< endl;
		cout << "cb          <file_path>  In  - codebook's path" << endl;
		cout << "o           <file_path>  Out - output's path " << endl;
		return -1;
	}
	//Parsing Options
	for (int i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-cb") == 0)
			CodeBookPath = argv[i + 1];
		else if (strcmp(argv[i], "-o") == 0)
			OutputFilePath = argv[i + 1];
		else {
			cout << "Wrong parameter" << endl;
			return -1;
		}
	}
	//Main process
	buildKdtree(CodeBookPath, OutputFilePath);
}
