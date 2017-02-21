#pragma once
#include <vector>
using namespace std;
class FuzzyC
{
private:
	int clusters;
	string wines_data[1011][306];
	vector<vector<double>> distance_data;
	
public:
	FuzzyC(int clusters, string wines[1011][306]);
	~FuzzyC();
	double JaccardDistance(int x1, int x2);
	vector<vector<double>> FeedData();
};

