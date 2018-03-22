/*
 * RetrieveImages_POEME.cpp
 *
 *  Created on: May 21, 2015
 *      Author: Neel
 */
//Access IUI All:
//g++ RetrieveImages_POEME.cpp -L /usr/lib -lopencv_core -lopencv_flann -o RetrieveImages_POEME -std=c++11

#include <tr1/unordered_map>
#include <sys/resource.h>
#include <opencv2/flann.hpp>
//#include <opencv/cv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <iterator>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <stdio.h>
#include <sstream>
#include <errno.h>
#include <string>
#include <vector>
#include <queue>
#include <set>

using namespace cv;
using namespace std;

struct Element {
	//vector<string> cents;
	vector<string> imageIds;
	float dist;
};

struct CandidateList {
	vector<Element> elms;
};

struct Pair {
	unsigned int idx_u;
	unsigned int idx_v;
	float dist;
};

class ComparePairs {
public:
	bool operator()(Pair& p1, Pair& p2) {
		return p1.dist >= p2.dist;
	}
};

struct imgFreq {
	string imageId;
	float freq;
	imgFreq(string imgId, float f) {
		imageId = imgId;
		freq = f;
	}
};

struct distances {
	float dmin;
	float dmax;
};

struct CandList_Data {
	vector<vector<vector<string> > > v_IUIs;
	vector<distances> v_dist;
	vector<cv::flann::Index *> v_kdtrees;
	vector<Mat> CodeBooks;
	CandList_Data(int size) {
		v_IUIs.resize(size);
		v_dist.resize(size);
		v_kdtrees.resize(size);
		CodeBooks.resize(size);
	}
	~CandList_Data() {
		for (unsigned int i = 0; i < v_kdtrees.size(); i++)
			delete v_kdtrees[i];
	}
};

class CompareFreq {
public:
	bool operator()(imgFreq& imgFreq1, imgFreq& imgFreq2) {
		return imgFreq1.freq < imgFreq2.freq;
	}
};

void split(string &str, char delim, vector<string> &elems) {
	stringstream data(str);
	string line;
	while (getline(data, line, delim))
		elems.push_back(line);
}

inline string formatInt(int num, int digits) {
	stringstream ss;
	ss << setw(digits) << setfill('0') << num;
	return ss.str();
}

inline double getUserTime(struct rusage *p) {
	if (getrusage(RUSAGE_SELF, p) != 0)
		cout << "Error while obtaining resources." << endl;
	return p->ru_utime.tv_sec + p->ru_utime.tv_usec / 1000000.0f;
}

int getdir(string dir, vector<string> &files) {
	files.clear();
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp = readdir(dp)) != NULL) {
		if (string(dirp->d_name) != "." && string(dirp->d_name) != "..")
			files.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

void getMaxMinDist(distances &dists, int k, string &root, string &dataset, string descriptor, string cb_size) {
	stringstream ss;
	ss << k;
	string str = ss.str();
	float testDist;	
	string path = root + "/5_ComputeDistances/data/" + dataset + "/" + descriptor + "/" + cb_size + "/Distances" + str + "nn.dat";
	//cout<<descriptor<<endl;
	ifstream fp(path.c_str());
	if (fp.is_open()) {
		fp >> dists.dmin;
		//cout<<"MIN: "<<dists.dmin<<endl;
		/*for (int i = 1; i <= k-1; i++)
		{
			fp >> dists.dmin;
			//cout<<"Minnn: "<<dists.dmin<<endl;
		}*/
		for (int i = 1; i <= k; i++)
		{
			fp >> dists.dmax;
			//cout<<"MAX: "<<dists.dmax<<endl;
		}		
		fp.close();
	}
}

void readCodeBook(Mat &Data, string DataPath) {
	ifstream fp(DataPath.c_str());
	int nkeys, dims;
	float val;
	if (fp.is_open()) {
		fp >> nkeys >> dims;
		Data = Mat(nkeys, dims, CV_32F);
		for (int row = 0; row < nkeys; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				Data.at<float>(row, col) = val;
			}
		}
		fp.close();
	}
	fp.close();
}

void readIUI(vector<vector<string> > &IUI, string &root, string &dataset, string &descriptor, string &cb_size) {
	string path = root + "/4_InvertedUniqueIndices/data/" + dataset + "/" + descriptor + "/InvertedUniqueIndices" + cb_size + ".dat";
	string data_line;
	ifstream fp(path.c_str());
	int i = 0;
	IUI.resize(atoi(cb_size.c_str()));
	if (fp.is_open()) {
		while (getline(fp, data_line)) {
			istringstream buf(data_line);
			istream_iterator<string> beg(buf), end;
			IUI[i].insert(IUI[i].begin(), beg, end);
			i++;
		}
		fp.close();
	}
}

//Joining a pair of elements from different lists

inline Pair makePair(CandidateList &list_u, CandidateList &list_v, int idx_u, int idx_v) {
	Pair p;
	p.idx_u = idx_u;
	p.idx_v = idx_v;

	if (list_u.elms.size() == 0) {
		cout << idx_u << "/" << list_u.elms.size() << endl;
		cout << idx_v << "/" << list_v.elms.size() << endl << endl;
	}
	p.dist = list_u.elms[idx_u].dist + list_v.elms[idx_v].dist;
	return p;
}

//Merging two elements from different Candidate Lists (corresponding to a pair)

Element getNewElement(CandidateList &list_u, CandidateList &list_v, Pair p) {
	Element e;

	//	e.cents.insert(e.cents.end(), list_u.elms[p.idx_u].cents.begin(), list_u.elms[p.idx_u].cents.end());
	e.imageIds.insert(e.imageIds.end(), list_u.elms[p.idx_u].imageIds.begin(), list_u.elms[p.idx_u].imageIds.end());

	//	e.cents.insert(e.cents.end(), list_v.elms[p.idx_v].cents.begin(),list_v.elms[p.idx_v].cents.end());
	e.imageIds.insert(e.imageIds.end(), list_v.elms[p.idx_v].imageIds.begin(), list_v.elms[p.idx_v].imageIds.end());

	e.dist = list_u.elms[p.idx_u].dist + list_v.elms[p.idx_v].dist;

	return e;
}

//Joins a pair of elements from different candidate lists and with a possible
// minimum distance, and insert them into a priority queue, until it finds T
// or a little more than T images related.
// (Read Inverted Multi-Index paper "Multi-Sequence Algorithm")

CandidateList multiSequencePair(CandidateList &list_u, CandidateList &list_v, unsigned int T) {
	priority_queue<Pair, vector<Pair>, ComparePairs> dist_queue;
	CandidateList CList;
	set<string> imageIDs;

	vector<unsigned int> last_idx_v(list_u.elms.size(), -1);

	Pair p;

	dist_queue.push(makePair(list_u, list_v, 0, 0));

	while (!dist_queue.empty() && imageIDs.size() < T) {
		p = dist_queue.top();
		dist_queue.pop();
		CList.elms.push_back(getNewElement(list_u, list_v, p));
		last_idx_v[p.idx_u] = p.idx_v;

		imageIDs.insert(list_u.elms[p.idx_u].imageIds.begin(), list_u.elms[p.idx_u].imageIds.end());
		imageIDs.insert(list_v.elms[p.idx_v].imageIds.begin(), list_v.elms[p.idx_v].imageIds.end());

		if (p.idx_u + 1 < list_u.elms.size() && (p.idx_v == 0 || last_idx_v[p.idx_u + 1] == p.idx_v - 1))
			dist_queue.push(makePair(list_u, list_v, p.idx_u + 1, p.idx_v));
		if (p.idx_v + 1 < list_v.elms.size() && (p.idx_u == 0 || last_idx_v[p.idx_u - 1] >= p.idx_v + 1))
			dist_queue.push(makePair(list_u, list_v, p.idx_u, p.idx_v + 1));
	}
	return CList;
}

//Applies a "Divide & Conquer" strategy to join more than 2 Candidate Lists.

CandidateList multiSequence(vector<CandidateList> &lists, unsigned int first, unsigned int last, unsigned int T) {
	assert(last >= first);
	if (last - first > 1) {
		unsigned int half = (first + last) / 2;
		CandidateList list_u = multiSequence(lists, first, half, T);
		CandidateList list_v = multiSequence(lists, half + 1, last, T);
		return multiSequencePair(list_u, list_v, T);
	}
	if (last - first == 1)
		return multiSequencePair(lists[first], lists[last], T);
	else
		//if(last-first==0)
		return lists[first];
}

void computeVoting(CandidateList &FinalList, tr1::unordered_map<string, float> &rankingFreq) {
	//Getting the frequencies
	for (unsigned int i = 0; i < FinalList.elms.size(); i++) {
		for (unsigned int j = 0; j < FinalList.elms[i].imageIds.size(); j++) {
			string imageId = FinalList.elms[i].imageIds[j];
			rankingFreq[imageId] += 1.0 - FinalList.elms[i].dist;
			//cout<<FinalList.elms[i].dist<<endl;
		}
	}
}

void getRanking(tr1::unordered_map<string, float> &rankingFreq, vector<string> &ranked_list, int verb, string output) {
	priority_queue<imgFreq, vector<imgFreq>, CompareFreq> imgFreq_queue;
	for (auto it = rankingFreq.begin(); it != rankingFreq.end(); ++it)
		imgFreq_queue.push(imgFreq(it->first, it->second));

	ofstream fp;
	if(verb)
	fp.open(output.c_str());
	while (!imgFreq_queue.empty()) {
		imgFreq iFreq = imgFreq_queue.top();
		ranked_list.push_back(iFreq.imageId);
		if(verb)
		{
			fp << iFreq.imageId << " " << iFreq.freq << endl;
			//cout<<iFreq.freq<<endl;
		}		
		imgFreq_queue.pop();
	}
	if(verb)
		fp.close();
}

/*
void createHtml(vector<string> &ranked_list, string root, string base, string dir_img, string output) {
	ofstream fp;
	string image;
	fp.open(output.c_str());

	string class_img = base.substr(2, 2);
	fp << "<html>" << endl;
	fp << "  <head>" << endl;
	fp << "    <title>Retrieved Images</title>" << endl;
	fp << "  </head>" << endl;
	fp << "  <body>" << endl;
	fp << "    <center>" << endl;
	fp << "      <h1>Image query</h1>" << endl;
	//fp << "      <p>Class:    " + class_img + "</p>" << endl;
	//fp << "      <p>Name:     " + base + "</p>" << endl;
	fp << "      <img src=\"" + dir_img + "/" + class_img + "/" + base + ".jpg\">" << endl;
	fp << "      <h2>Images retrieved</h2>" << endl;
	fp << "      <table border=\"1\">" << endl;

	int pos = 1;
	for (unsigned int i = 0; i < ranked_list.size(); i++) {
		image = ranked_list[i];
		class_img = image.substr(2, 2);
		if (pos % 6 == 1)
			fp << "        <tr>" << endl;

		fp << "          <td>" << endl;
		fp << "            <p><b>Position:</b>    " + to_string(pos) + "</p>";
		fp << endl;
		//fp << "            <p><b>Class:</b>    " + class_img + "</p>" << endl;
		fp << "            <p><b>Name:</b>     " + image + "</p>" << endl;
		fp << "            <img src=\"" + dir_img + "/" + class_img + "/" + image + ".jpg\">" << endl;
		fp << "          </td>" << endl;
		if (pos % 6 == 0)
			fp << "        </tr>" << endl;
		pos++;
	}

	fp << "      </table>" << endl;
	fp << "    </center>" << endl;
	fp << "  </body>" << endl;
	fp << "</html>" << endl;
	fp.close();
}
*/

vector<string> load_list(const string& fname) {
	vector<string> ret;
	ifstream fobj(fname.c_str());
	string line;
	while (getline(fobj, line)) {
		ret.push_back(line);
	}
	return ret;
}

template<class T>
set<T> vector_to_set(const vector<T>& vec) {
	return set<T>(vec.begin(), vec.end());
}

inline string getFilePath(string &dir_out, string &img, string &cb_size, string &descriptor, string &knn, string ext) {
	return dir_out + "/Res_" + img + "_" + descriptor + "_" + cb_size + "w_" + knn + "nn." + ext;
}

cv::flann::Index * loadKdtree(string &root, string &dataset, string &descriptor, string &cb_size, Mat &CodeBook) {
	string CodeBookPath = root + "/3_CodeBooks/data/" + dataset + "/" + descriptor + "/CodeBook" + cb_size + ".dat";
	string KdtreePath = root + "/3_CodeBooks/data/" + dataset + "/" + descriptor + "/Kdtree" + cb_size + ".dat";
	readCodeBook(CodeBook, CodeBookPath);
	return new cv::flann::Index(CodeBook, cv::flann::SavedIndexParams(KdtreePath), cvflann::FLANN_DIST_EUCLIDEAN);
}

struct desc_data {
	int nkeys;
	vector<ifstream *> fps;
	vector<Mat> QueryMats;
	desc_data(unsigned int size) {
		nkeys = 0;
		fps.resize(size);
		for (unsigned int i = 0; i < size; i++)
			fps[i] = new ifstream;
		QueryMats.resize(size);
	}
	~desc_data() {
		for (unsigned int i = 0; i < fps.size(); i++)
			delete fps[i];
	}
};

void computeMSRank(string root, string dataset, string detector, string descriptor, string cb_size, string knn, int t, string list_classes, int verb) {
	cout << endl << "Retrieving images from " << dataset << " dataset. Detector: "<<detector<<", Descriptors: " << descriptor << ", CodebookSizes: " << cb_size << "cw and " << knn + "-NN" << endl << endl;
	struct rusage usage;
	vector<string> CandLists_nm;
	vector<string> descriptors;
	vector<string> detectors;
	vector<string> cb_sizes;
	vector<string> classes;
	vector<string> images;
	vector<string> imagID;
	vector<float> apImg;
	vector<float> t_precision;
	vector<float> t_recall;
	int cont = 0;
	string dir_in, dir_base_out, dir_out, command, dir_out_rank, dir_out_html, dir_out_res;
	t_precision.resize(101);
	t_recall.resize(101);
	split(descriptor, '-', descriptors);
	split(detector, '-', detectors);
	split(cb_size, '-', cb_sizes);
	split(list_classes, '-', classes);
	int knn_val = atoi(knn.c_str());
	CandList_Data CList_Data(descriptors.size());
	cout << "   Loading files..." << endl;
	double time = getUserTime(&usage);
		
	if (detectors.size() > 1)
	{		
		for (unsigned int i = 0; i < descriptors.size(); i++) {
			if (detector != "")
				descriptors[i] = detectors[0] + "+" + descriptors[i] + "+" + detectors[1] + "+" + descriptors[i];
			readIUI(CList_Data.v_IUIs[i], root, dataset, descriptors[i], cb_sizes[i]);
			getMaxMinDist(CList_Data.v_dist[i], knn_val, root, dataset, descriptors[i], cb_sizes[i]);
			CList_Data.v_kdtrees[i] = loadKdtree(root, dataset, descriptors[i], cb_sizes[i], CList_Data.CodeBooks[i]);
		}
	}

	else
	{
		for (unsigned int i = 0; i < descriptors.size(); i++) {
			if (detector != "")
				descriptors[i] = detector + "+" + descriptors[i];
			readIUI(CList_Data.v_IUIs[i], root, dataset, descriptors[i], cb_sizes[i]);
			getMaxMinDist(CList_Data.v_dist[i], knn_val, root, dataset, descriptors[i], cb_sizes[i]);
			CList_Data.v_kdtrees[i] = loadKdtree(root, dataset, descriptors[i], cb_sizes[i], CList_Data.CodeBooks[i]);
		}
	}
	//cout<<"Min: "<<CList_Data.v_dist[0].dmin<<"Max: "<<CList_Data.v_dist[0].dmax<<endl;
	//cout << "      Time: " << getUserTime(&usage) - time << endl << endl;
	time = getUserTime(&usage);
	cout << "   Starting retrieval process... " << endl;
	/******************************************************************************/
	dir_base_out = root + "/Results/" + dataset;
	mkdir(dir_base_out.c_str(), 0777);
	if (verb) {
		dir_base_out = dir_base_out + "/" + detector + "+" + descriptor;
		mkdir(dir_base_out.c_str(), 0777);
	}
	/******************************************************************************/
	time = getUserTime(&usage);
	/*for each class*/
	for (unsigned int i = 0; i < classes.size(); i++) {
		cout << "      Computing results for class " << classes[i] << endl;
		/**************************************************************************/
		if (verb) {
			dir_out = dir_base_out + "/" + classes[i];
			mkdir(dir_out.c_str(), 0777);
		}
		/**************************************************************************/
		dir_in = root + "/0_ImgDatasets/" + dataset + "/color/" + classes[i];
		getdir(dir_in, images);
		/*for each image*/
		for (unsigned int j = 0; j < images.size(); j++) {
			//string img = images[j].substr(0, 9);
			string img = images[j].substr(0, images[j].length()-4);
			/**************************************************************************/
			if (verb) {
				dir_out = dir_base_out + "/" + classes[i] + "/" + img;
				mkdir(dir_out.c_str(), 0777);
				dir_out += "/" + cb_size + "cw";
				mkdir(dir_out.c_str(), 0777);
			}
			/**************************************************************************/
			cont++;
			vector<float> precision;
			vector<float> recall;
			
			desc_data d_data(descriptors.size());
			int dims;
			for (unsigned int k = 0; k < descriptors.size(); k++) {
				string path = root + "/2_Descriptors/data/" + dataset + "/" + descriptors[k] + "/" + classes[i] + "/" + img + ".desc";
				d_data.fps[k]->open(path.c_str());
				*d_data.fps[k] >> d_data.nkeys >> dims;
				d_data.QueryMats[k] = Mat(1, dims, CV_32F);
			}
			/*for each query*/
			tr1::unordered_map<string, float> rankingFreq;
			for (int q = 0; q < d_data.nkeys; q++) {
				vector<CandidateList> CandidateLists(descriptors.size());
				bool empty = false;
				for (unsigned int k = 0; k < descriptors.size(); k++) {
					for (int col = 0; col < d_data.QueryMats[k].cols; col++)
						*d_data.fps[k] >> d_data.QueryMats[k].at<float>(0, col);
					Mat Indices;
					Mat Dists;

					CList_Data.v_kdtrees[k]->knnSearch(d_data.QueryMats[k], Indices, Dists, knn_val, cv::flann::SearchParams(64));
					CandidateLists[k].elms.resize(knn_val);
					set<string> images;
					int count = 0;
					for (int col = 0; col < Indices.cols; col++) {
						int idx = Indices.at<int>(0, col);
						for (unsigned int l = 0; l < CList_Data.v_IUIs[k][idx].size(); l++) {
							if (!images.count(CList_Data.v_IUIs[k][idx][l])) {
								CandidateLists[k].elms[count].imageIds.push_back(CList_Data.v_IUIs[k][idx][l]);
								images.insert(CList_Data.v_IUIs[k][idx][l]);
							}
						}
						if (CandidateLists[k].elms[count].imageIds.size() > 0) {
							CandidateLists[k].elms[count].dist = (Dists.at<float>(0, col) - CList_Data.v_dist[k].dmin) / (CList_Data.v_dist[k].dmax - CList_Data.v_dist[k].dmin);
							count++;
						}
					}
					CandidateLists[k].elms.resize(count);
					if (count == 0) {
						empty = true;
						break;
					}
				}
				if (empty)
					continue;
				CandidateList FinalList = multiSequence(CandidateLists, 0, CandidateLists.size() - 1, t);
				/*Voting*/
				computeVoting(FinalList, rankingFreq);
			}
			for (unsigned int l = 0; l < descriptors.size(); l++)
				d_data.fps[l]->close();
			
			vector<string> ranked_list;
			dir_out_rank = getFilePath(dir_out, img, cb_size, descriptor, knn, "rank");
			getRanking(rankingFreq, ranked_list, verb, dir_out_rank);
			if (verb) {
				dir_out_html = getFilePath(dir_out, img, cb_size, descriptor, knn, "html");
				//createHtml(ranked_list, root, img, dir_img, dir_out_html);
			}
		}
	}
}

int main(int argc, char *argv[]) 
{
	//Defining variables
	string root;
	string dataset;
	string detector;
	string descriptor;
	string cb_sizes;
	string k;
	//string dir_img;
	int t = 0, verb = 0;
	string classes;

	//USAGE message
	if (argc != 19) {
		cout << "USAGE: " << argv[0] << " -dir <Directory> -dat <Dataset> -det <Detector> -desc <FusionDescriptor> -cw <CodebookSizes> -k <#NearesNeighbour> -t <ret_img> -c <classes> -dir_img <ImagesPath> -verb 1" << endl;
		cout << "dir         In  - directory to the IGN folder." << endl;
		cout << "dat         In  - name of the dataset used." << endl;
		cout << "det         In  - name of the detector used (can be empty)." << endl;
		cout << "desc        In  - name of descriptors to fusion (e.g.: SIFT-SURF-SC)." << endl;
		cout << "cw          In  - codebook's sizes of the descriptors (e.g.: 250-200-200)." << endl;
		cout << "k           In  - quantity of nearest neighbors to use." << endl;
		cout << "t           In  - maximum number of images to retrieve." << endl;
		cout << "c           In  - list of the classes to be computed (e.g.: 01-02-05)." << endl;
		//cout << "dir_img     In  - directory to find the images to show in the html results." << endl;
		cout << "verb        In  - [1|0] print detail html and results files." << endl << endl;
		return -1;
	}

	//Parsing Options
	for (int i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-dir") == 0)
			root = argv[i + 1];
		else if (strcmp(argv[i], "-dat") == 0)
			dataset = argv[i + 1];
		else if (strcmp(argv[i], "-det") == 0)
			detector = argv[i + 1];
		else if (strcmp(argv[i], "-desc") == 0)
			descriptor = argv[i + 1];
		else if (strcmp(argv[i], "-cw") == 0)
			cb_sizes = argv[i + 1];
		else if (strcmp(argv[i], "-k") == 0)
			k = argv[i + 1];
		else if (strcmp(argv[i], "-t") == 0)
			t = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-c") == 0)
			classes = argv[i + 1];
		//else if (strcmp(argv[i], "-dir_img") == 0)
		//	dir_img = argv[i + 1];
		else if (strcmp(argv[i], "-verb") == 0)
			verb = atoi(argv[i + 1]);
		else {
			cout << "Wrong parameter: " << argv[i] << endl;
			return -1;
		}
	}

	//Main process
	computeMSRank(root, dataset, detector, descriptor, cb_sizes, k, t, classes, verb);
}
