#pragma once
#include <vector>
using namespace std;
class FuzzyC
{
private:
	int clusters;
	vector<vector<double>> distance_data;
public:
	FuzzyC(int clusters, vector<vector<double>> &data);
	~FuzzyC();
};

