#pragma once
#include <vector>
using namespace std;
class FuzzyC
{
private:
	int clusters;
	vector<vector<double>> distance_data;
	vector<vector<string>> wines_data;
public:
	FuzzyC(int clusters, string wines[1011][306]);
	~FuzzyC();
	double JaccardDistance(int x1, int x2);
	vector<vector<double>> FeedData();
};

