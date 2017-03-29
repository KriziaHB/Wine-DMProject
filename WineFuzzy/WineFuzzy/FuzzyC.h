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
	double termination_criterion = 0.5;
	vector<double> threshold;
	vector<int> cluster_points;
	vector<vector<double>> distance_data;
	vector<vector<double>> wines_data;
	vector<vector<double>> membership_data;
	vector<vector<double>> prev_membership_data;
	double avgAttributes; 
	int fold = 5; 
	int mod = 0; 
public:
	FuzzyC(int clusters, int m, string wines[1011][306]);
	~FuzzyC();
	void initializeClusters();
	bool checkTermination();
	double jaccardDistance(int x1, int x2);
	vector<double> calculateThreshold();
	vector<vector<double>> calculateDistance();
	vector<vector<double>> initializeMembership();
	vector<vector<double>> storeIteration();
	double calculateMembership(int wine, int cluster);
	double calculateCentroid(int col, int cluster);
	double roundCentroid(int col, double centroid);
	double calculateConvergence(int cluster, int wine);
	void generateCenters(); 
	double tallyAttributes(); 
	vector<double> manhattan2(int cluster, vector<double> e);
	void writeToFile(); 
};
