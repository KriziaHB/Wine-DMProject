#include "FuzzyC.h"
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <string>

using namespace std;


FuzzyC::FuzzyC(int clusters, int m_value, string wines[1011][306])
{
	this->clusters = clusters;
	vector<string> element;
	for (int i = 1; i < 1010; i++) {
		for (int j = 1; j < 305; j++) {
			element.push_back(wines[i][j]);
		}
		wines_data.push_back(element);
		element.clear();
	}
	m = m_value;
	FeedData();
	initializeClusters();
	initializeMembership();
	generateCenters();
}


FuzzyC::~FuzzyC()
{
}

double FuzzyC::JaccardDistance(int x1, int x2) {
	double a = 0.0; //# of attributes of A is 0 while 1 in B && # of attributes of A is 1 while 0 in B 
	double b = 0.0; //# of attributes where A and B have value of 1 
	double dist = 0.0;

	// count up a (different values) and b (both have 1) between the two points 
	for (int i = 0; i < 303; i++) {
		if ((wines_data[x1].at(i) == "1") && (wines_data[x2].at(i) == "1"))
			b++;
		else if (wines_data[x1].at(i) != wines_data[x2].at(i))
			a++;
	}

	//Formula: simplified by adding together both cases of 1 vs 0 when comparing wines 
	dist = double(a / (a + b));
	return(dist);
}

void FuzzyC::initializeClusters() {
	for (int i = 0; i < clusters; i++) {
		cluster_points.push_back(rand() % 1010 + 1);
	}
	//for test purposes
	cluster_points[1] = 20;
}

vector<vector<double>> FuzzyC::initializeMembership() {
	vector<double> element;
	membership_data.clear();
	for (int i = 0; i < distance_data.size(); i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(CalculateMembership(i, cluster_points[j]));
		}
		membership_data.push_back(element);
		element.clear();
	}
	return membership_data;
}
void FuzzyC::generateCenters() {
	vector<string> element;
	for (int i = 0; i < cluster_points.size(); i++) {
		for (int j = 0; j < 304; j++) {
			element.push_back(to_string(calculateCentroid(j, i)));
		}
		cluster_points[i] = wines_data.size();
		wines_data.push_back(element);
		element.clear();
	}
}

double FuzzyC::calculateCentroid(int col, int cluster) {
	double numerator = 0;
	double denominator = 0;
	double centroid = 0;
	for (int i = 0; i < distance_data.size(); i++) {
		numerator += pow(membership_data[i].at(cluster), m) * stod(wines_data[i].at(col));
		denominator += pow(membership_data[i].at(cluster), m);
	}
	centroid = numerator / denominator;
	return centroid;
}

double FuzzyC::CalculateMembership(int wine, int cluster){
	double value = 0;
	for (int i = 0; i < cluster_points.size(); i++) {
		if (wine == cluster_points[i]) {
			value = 1;
			return value;
		}
	}
	double distance_to_cluster = distance_data[wine].at(cluster);
	double power_value = 2 / (m - 1);
	for (int i = 0; i < cluster_points.size() ; i++) {
		value += pow((distance_to_cluster / distance_data[wine].at(cluster_points[i])), power_value);
	}
	value = 1 / value;

	return value;
}
vector<vector<double>> FuzzyC::FeedData() {

	vector<double> element;
	for (int i = 1; i < 50; i++) {
		for (int j = 1; j < 50; j++) {
			element.push_back(JaccardDistance(i, j));
		}
		distance_data.push_back(element);
		element.clear();
	}
	return distance_data;

}
