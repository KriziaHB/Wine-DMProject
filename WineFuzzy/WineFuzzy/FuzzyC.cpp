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
	for (int i = 1; i < 1010; i++) {
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
//	initializeMembership();


	//each wine membership then calculate centroid separately 
	for (int i = 0; i < 50; i++) {
		initializeMembership(i); 
		generateCenters(i); 
	}

	//Generate new cluster centers based on membership values
//	generateCenters();
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
	//Select random wines as the centers
	for (int i = 0; i < clusters; i++) {
		cluster_points.push_back(rand() % 1010 + 1);
	}
	//for test purposes
	cluster_points[1] = 20;
}

vector<vector<double>> FuzzyC::initializeMembership(int row) {
	//Generate Jaccard's distance
	FeedData();
	//Clear old membership data (This is for calculating new values)
	membership_data.clear();
	vector<double> element;
/*	for (int i = 0; i < distance_data.size(); i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(CalculateMembership(i, cluster_points[j])); //Calculate membership value for wine to each cluster point
		}
		membership_data.push_back(element);//Collect both cluster points for single wine and loop
		element.clear();
	}
	return membership_data;
	*/

	//KHB
	for (int j = 0; j < clusters; j++) {
		element.push_back(CalculateMembership(row, j)); //Calculate membership value for wine to each cluster point
	}
	membership_data.push_back(element);//Collect both cluster points for single wine and loop
	element.clear();

	return membership_data; //For one row / wine at a time 
}

void FuzzyC::generateCenters(int row) { //85 & 86 ? KHB
	vector<double> element;
	for (int i = 0; i < cluster_points.size(); i++) {
				//KHB	int i = row; 
		for (int j = 0; j < 304; j++) {
//			element.push_back(to_string(calculateCentroid(j, i)));//For each attribute calculate the centroid (or average of all points in attribute)
			element.push_back(calculateCentroid(j, i));//For each attribute calculate the centroid (or average of all points in attribute)
		}
//		cluster_points[i] = wines_data.size(); //Assign new index as the new cluster point
//		wines_data.push_back(element); //Append new centroid to wine data

		//append new centroid data as doubles for now 
		cluster_centroids.push_back(element); 
		element.clear();
	}

	//Use generated centroid info to find closest actual wine and replace as medoid 
	for (int i = 0; i < clusters; i++) {
		Manhattan(i);
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

double FuzzyC::CalculateMembership(int wine, int cluster) {
	double value = 0;
	//Check if current wine is in cluster
	for (int i = 0; i < clusters; i++) {
		if (wine == cluster_points[i]) {
			value = 1;
			return value;
		}
	}
	double distance_to_cluster = distance_data[wine].at(cluster);
	double power_value = 2 / (m - 1);
	for (int i = 0; i < cluster_points.size(); i++) {
		value += pow((distance_to_cluster / distance_data[wine].at(cluster_points[i])), power_value);//summation of all distance to cluster points
	}
	value = 1 / value;

	return value;
}

vector<vector<double>> FuzzyC::FeedData() {
	vector<double> element;
	for (int i = 1; i < 50; i++) {
		for (int j = 1; j < 50; j++) {
			element.push_back(JaccardDistance(i, j)); //Compare each wine to each other wine, find the distance
		}
		distance_data.push_back(element); //Store distance of each wine to wine index
		element.clear();
	}
	return distance_data;
}



//KHB - use generated centroids to find closest real wine and 
//		replace cluster_points indices with new found closest real wine medoids 
void FuzzyC::Manhattan(int c) {
	manDists.clear(); 
	double element = 0.0; 
	for (int i = 0; i < 50; i++) { //iterate through each test wine
		for (int attr = 0; attr < 305; attr++) { //iterate through attributes
			int val = 0; 
			if (wines_data[i].at(attr) == "1")					
				val = 1; 

			//add up absolute values of distance at each attribute 
			element += abs(cluster_centroids[c].at(i) - val);
		}
		//all attribute distances at each wine 
		manDists.push_back(element);
		element = 0.0; 
	}
	// cluster's medoid is now a real wine as selected from shortest distance to generated centroid  
	cluster_points[c] = minimumDistance();
}
//KHB - return index of wine shortest distance from generated centroid 
int FuzzyC::minimumDistance() {
	int min = manDists.at(0);
	int index = 0; 
	for (int i = 1; i < manDists.size(); i++) {
		if (manDists.at(i) < min) {
			min = manDists.at(i); 
			index = i; 
		} 
	}
	return (index); 
}