#include "FuzzyC.h"
#include <vector>

using namespace std;


FuzzyC::FuzzyC(int clusters, vector<vector<double>> &data)
{
	this->clusters = clusters;
	distance_data = data;
}


FuzzyC::~FuzzyC()
{
}

