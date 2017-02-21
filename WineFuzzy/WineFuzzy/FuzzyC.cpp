#include "FuzzyC.h"
#include <vector>

using namespace std;


FuzzyC::FuzzyC(int clusters, string wines[1011][306])
{
	this->clusters = clusters;
	vector<string> element;
	for (int i = 0; i < 1101; i++) {
		for (int j = 0; j < 306; j++) {
			element.push_back(wines[i][j]);
		}
		wines_data.push_back(element);
		element.clear();
	}
}


FuzzyC::~FuzzyC()
{
}

double FuzzyC::JaccardDistance(int x1, int x2) {
	double a = 0.0; //# of attributes of A is 0 while 1 in B && # of attributes of A is 1 while 0 in B 
	double b = 0.0; //# of attributes where A and B have value of 1 
	double dist = 0.0;

	// count up a (different values) and b (both have 1) between the two points 
	for (int i = 1; i < 306; i++) {
		if ((wines_data[x1].at(i) == "1") && (wines_data[x2].at(i) == "1"))
			b++;
		else if (wines_data[x1].at(i) != wines_data[x2].at(i))
			a++;
	}

	//Formula: simplified by adding together both cases of 1 vs 0 when comparing wines 
	dist = double(a / (a + b));
	return(dist);
}

vector<vector<double>> FuzzyC::FeedData() {

	vector<double> element;

	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 10; j++) {
			element.push_back(JaccardDistance(i, j));
		}
		distance_data.push_back(element);
		element.clear();
	}

	return distance_data;
}