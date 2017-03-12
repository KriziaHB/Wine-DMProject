#pragma once
#include <vector>
using namespace std;
class FuzzyC
{
private:
	const int INITIAL_COL = 304;
	const int INITIAL_ROW = 1010;
	int clusters;
	int m;
	vector<double> threshold;
	vector<int> cluster_points;
	vector<vector<double>> distance_data;
	vector<vector<string>> wines_data;
	vector<vector<double>> membership_data;
public:
	FuzzyC(int clusters, int m, string wines[1011][306]);
	~FuzzyC();
	void initializeClusters();
	double jaccardDistance(int x1, int x2);
	vector<double> calculateThreshold();
	vector<vector<double>> calculateDistance();
	vector<vector<double>> initializeMembership();
	double calculateMembership(int wine, int cluster);
	double calculateCentroid(int col, int cluster);
	double roundCentroid(int col, double centroid);
	void generateCenters();

};
