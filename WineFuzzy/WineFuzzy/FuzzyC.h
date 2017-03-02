#pragma once
#include <vector>
using namespace std;
class FuzzyC
{
private:
	int clusters; // k 
	int m;
	vector<int> cluster_points; //Real wines as initial centroids 
	vector<vector<double>> distance_data; // distance matrix 
	vector<vector<string>> wines_data; // copied 2D vector of wines and their attributes 
	//KHB changed to single vector 
	vector<double> membership_data; // percentages each index (wine) belongs to each cluster (column) 
	vector<vector<double>> cluster_centroids; //KHB centroids per cluster 
	vector<double> manDists; //KHB distances derived from Manhattan calculation 
public:
	FuzzyC(int clusters, int m, string wines[1011][306]);
	~FuzzyC();
	void initializeClusters();
	double JaccardDistance(int x1, int x2);
	vector<vector<double>> FeedData();
	vector<double> initializeMembership(int row); //KHB changed return type 
	double CalculateMembership(int wine, int cluster);
	double calculateCentroid(int col, int cluster);
	void generateCenters(int row);
	void Manhattan(int c);
	int minimumDistance(); 

};
