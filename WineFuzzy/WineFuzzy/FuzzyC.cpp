#include "FuzzyC.h"
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <string>
using namespace std;


FuzzyC::FuzzyC(int clusters, int m_value, string wines[1011][306])
{
	//Take number of clusters
	this->clusters = clusters;
	//Copy Wine Data Over - Trimmed
	vector<string> element;
	for (int i = 1; i < 1011; i++) {
		for (int j = 1; j < 305; j++) {
			element.push_back(wines[i][j]);
		}
		wines_data.push_back(element);
		element.clear();
	}
	//Sets the m value for fuzzyness
	m = m_value;
	//Generate random clusters based on current points
	initializeClusters();
	//Find the membership value for each wine to each cluster
	initializeMembership();
	//Generate new cluster centers based on membership values
	generateCenters();
	//Check to see if Algorithm is ready to terminate
	checkTermination();
}

FuzzyC::~FuzzyC()
{
}

double FuzzyC::jaccardDistance(int x1, int x2) {
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
	//Select random wines as the centers
	for (int i = 0; i < clusters; i++) {
		cluster_points.push_back(rand() % wines_data.size());
	}
}

vector<vector<double>> FuzzyC::initializeMembership() {
	//Generate Jaccard's distance
	calculateDistance();
	calculateThreshold();
	//Clear old membership data (This is for calculating new values)
	membership_data.clear();
	vector<double> element;
	for (int i = 0; i < distance_data.size(); i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(calculateMembership(i, j)); //Calculate membership value for wine to each cluster point
		}
		membership_data.push_back(element);//Collect both cluster points for single wine and loop
		element.clear();
	}
	return membership_data;
}

void FuzzyC::generateCenters() {
	vector<string> element;
	for (int i = 0; i < cluster_points.size(); i++) {
		for (int j = 0; j < INITIAL_COL; j++) {
			element.push_back(to_string(calculateCentroid(j, i)));//For each attribute calculate the centroid (or average of all points in attribute)
		}
		cluster_points[i] = wines_data.size(); //Assign new index as the new cluster point
		wines_data.push_back(element); //Append new centroid to wine data
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
	//Round the centroid up or down based on whether it meets the threshold requirement
	centroid = roundCentroid(col, centroid);
	return centroid;
}

double FuzzyC::calculateMembership(int wine, int cluster) {
	double value = 0;
	//Check if current wine is in cluster
	for (int i = 0; i < cluster_points.size(); i++) {
		if (wine == cluster_points[i]) {
			value = 1;
			return value;
		}
	}
	double distance_to_cluster = distance_data[wine].at(cluster);
	double power_value = 2 / (m - 1);
	for (int i = 0; i < cluster_points.size(); i++) {
		value += pow((distance_to_cluster / distance_data[wine].at(i)), power_value);//summation of all distance to cluster points
	}
	value = 1 / value;

	return value;
}

vector<vector<double>> FuzzyC::calculateDistance() {
	distance_data.clear();
	vector<double> element;
	for (int i = 0; i < INITIAL_ROW; i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(jaccardDistance(i, cluster_points[j])); //Compare each wine to each other wine, find the distance
		}
		distance_data.push_back(element); //Store distance of each wine to wine index
		element.clear();
	}
	return distance_data;
}

vector<double> FuzzyC::calculateThreshold() {
	threshold.clear();
	double buffer;
	//Find the average of each data's attribute and return into a vector
	for (int i = 0; i < INITIAL_COL; i++) {
		buffer = 0;
		for (int j = 0; j < INITIAL_ROW; j++) {
			buffer += stod(wines_data[j][i]);
		}
		threshold.push_back(buffer / INITIAL_ROW);
	}
	return threshold;
}
double FuzzyC::roundCentroid(int col, double centroid) {
	//check if centroid meets threshold requirement
	if (centroid >= threshold[col]) {
		return 1;
	}
	else {
		return 0;
	}
}
bool FuzzyC::checkTermination() {
	double max = 0;
	for (int i = 0; i < cluster_points.size(); i++) {
		for (int j = membership_data.size() - 1; j > 1; j--) {
			if (calculateConvergence(i, j) > max) {
				max = calculateConvergence(i, j);
			}
		}
		if (max < termination_criterion) {
			return false;
		}
	}
	return true;
}
double FuzzyC::calculateConvergence(int cluster, int wine) {
	double buffer = abs(membership_data[wine][cluster] - membership_data[wine - 1][cluster]);
	return buffer;
}