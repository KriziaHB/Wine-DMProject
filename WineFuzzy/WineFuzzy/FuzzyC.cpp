#include "FuzzyC.h"
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream> 
#include <fstream> 
#include <iterator>
using namespace std;


FuzzyC::FuzzyC(int clusters, int m_value, string wines[1011][306])
{
	//Take number of clusters
	this->clusters = clusters;
	//Copy Wine Data Over - Trimmed
	vector<double> element;
	for (int i = 1; i < 1011; i++) {
		for (int j = 1; j < 305; j++) {
			element.push_back(stod(wines[i][j]));
		}
		wines_data.push_back(element);
		element.clear();
	}
	//Sets the m value for fuzzyness
	m = m_value;

	run();
	fiveFoldTest();

	printf("Done");
}

FuzzyC::~FuzzyC()
{
}
//Calculate distance from each wine to each centroid
double FuzzyC::jaccardDistance(int x1, int x2, bool test) {

	double a = 0.0; //# of attributes of A is 0 while 1 in B && # of attributes of A is 1 while 0 in B 
	double b = 0.0; //# of attributes where A and B have value of 1 
	double dist = 0.0;

	if(!test){
		// count up a (different values) and b (both have 1) between the two points 
		for (int i = 0; i < 303; i++) {
			if ((collapsed_wines_data[x1].at(i) == 1) && (collapsed_wines_data[x2].at(i) == 1))
				b++;
			else if (collapsed_wines_data[x1].at(i) != collapsed_wines_data[x2].at(i))
				a++;
		}
	}
	else {
		// count up a (different values) and b (both have 1) between the two points 
		for (int i = 0; i < 303; i++) {
			if ((removed_wines_data[x1].at(i) == 1) && (collapsed_wines_data[x2].at(i) == 1))
				b++;
			else if (removed_wines_data[x1].at(i) != collapsed_wines_data[x2].at(i))
				a++;
		}
	}

	//Formula: simplified by adding together both cases of 1 vs 0 when comparing wines 
	dist = double(a / (a + b));
	return(dist);
}
//Select random wines as the centers for first iteration
void FuzzyC::initializeClusters() {
	cluster_points.clear();
	for (int i = 0; i < clusters; i++) {
		cluster_points.push_back(rand() % collapsed_wines_data.size());
	}
}
//Generate Membership % to each centroid
vector<vector<double>> FuzzyC::initializeMembership() {
	//Generate Jaccard's distance
	calculateDistance();
	//Calculate the threshold for 1/0 cutoff for centroids
	calculateThreshold();
	//Store the previous iteration of membership_data for convergence calculations in checkTermination()
	storeIteration();
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
//Append new centroids to wines_data for ease of calculation
void FuzzyC::generateCenters() {
	vector<double> element;
	for (int i = 0; i < cluster_points.size(); i++) {

		//Both options of Manhattan (rounded & tally up) 
		if (option == 1) {
			for (int j = 0; j < INITIAL_COL; j++) {
				element.push_back(calculateCentroid(j, i));//For each attribute calculate the centroid (or average of all points in attribute)
			}

			cluster_points[i] = collapsed_wines_data.size(); //Assign new index as the new cluster point
			collapsed_wines_data.push_back(element); //Append new centroid to wine data
		}
		else {
			//[KHB] option #2 for Manhattan tally up
			cluster_points[i] = collapsed_wines_data.size(); //Assign new index as the new cluster point
			
			for (int a = 0; a < INITIAL_COL; a++) {
				//For each attribute calculate the centroid (or average of all points in attribute)
				//Utilize rounding for tally 
				element.push_back(calculateCentroid(a, i));
			}
			wines_data.push_back(manhattan2(i, element));
		}

		element.clear();
	}
}
//Calculate the new centroid based on Fuzzy C algorithm for each attribute
double FuzzyC::calculateCentroid(int col, int cluster) {
	double numerator = 0;
	double denominator = 0;
	double centroid = 0;
	for (int i = 0; i < distance_data.size(); i++) {
		numerator += pow(membership_data[i].at(cluster), m) * (collapsed_wines_data[i].at(col));
		denominator += pow(membership_data[i].at(cluster), m);
		if (isnan(numerator)) {
			break;
		}
	}
	centroid = numerator / denominator;

	//Round the centroid up or down based on whether it meets the threshold requirement
	centroid = roundCentroid(col, centroid);

	return centroid;
}
//Find the membership value to the centroid using Fuzzy C Means with Jaccard's distance
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
//Find the distance of each point to the current centroid (Uses Jaccards Distance)
vector<vector<double>> FuzzyC::calculateDistance() {
	distance_data.clear();
	vector<double> element;
	for (int i = 0; i < INITIAL_ROW; i++) {
			for (int j = 0; j < cluster_points.size(); j++) {
				element.push_back(jaccardDistance(i, cluster_points[j], false)); //Compare each wine to each other wine, find the distance
			}
			distance_data.push_back(element); //Store distance of each wine to wine index
			element.clear();
	}
	return distance_data;
}
//Find the average of each attribute and store it in a vector
vector<double> FuzzyC::calculateThreshold() {
	threshold.clear();
	double buffer;
	//Find the average of each data's attribute and return into a vector
	for (int i = 0; i < INITIAL_COL; i++) {
		buffer = 0;
		for (int j = 0; j < INITIAL_ROW; j++) {
			buffer += collapsed_wines_data[j][i];
		}
		threshold.push_back(buffer / INITIAL_ROW);
	}
	return threshold;
}
//Round the centroid values (which are currently decimal) to hard 1/0 values based on threshold calculation
double FuzzyC::roundCentroid(int col, double centroid) {
	//check if centroid meets threshold requirement
	if (centroid >= threshold[col]) {
		return 1;
	}
	else {
		return 0;
	}
}
//Check to see whether changes in membership values are changing enough
//If there is little change there doesn't need to be another iteration
//return false for do-while in constructor will shut off algorithm
bool FuzzyC::checkTermination() {
	double max = 0;
	for (int i = 0; i < cluster_points.size(); i++) {
		for (int j = membership_data.size() - 1; j > 0; j--) {
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
//Check the current values of membership_data to the previous iterations for convergence calculations
double FuzzyC::calculateConvergence(int cluster, int wine) {
	double buffer = abs(membership_data[wine][cluster] - prev_membership_data[wine][cluster]);
	return buffer;
}
//Store current iteration for future convergence calculations
vector<vector<double>> FuzzyC::storeIteration() {
	prev_membership_data.clear();
	prev_membership_data = membership_data;
	return prev_membership_data;
}


//[KHB] tally attributes of each wine and get the average to be used for OPTION #2 Manhattan 
double FuzzyC::tallyAttributes() {

	double avgTally = 0.0; 
	int totalTally = 0; 

	//Tally up all ones in data set then divide by number of wines to find average ones per wine 
	for (int i = 0; i < 1011; i++) {
		int tally = 0; 
		for (int j = 0; j < 304; j++) { 
			tally += wines_data[i].at(j); 
		}
		totalTally += tally; 
	}
	avgTally = totalTally / 1010;

	return(avgTally); 
}
//Take average number of attributes and force a wine to match all attributes (if no wine then to match -1  each iteration until 1 attribute matches) 
//Return clone of existing wine to be appended to wines_data as centroid/medoid 
vector<double> FuzzyC::manhattan2(int cluster, vector<double> e)
{	
	//iterate through starting from matching average number of attributes to 1
	for (int min = avgAttributes; min > 0; min--)
	{
		for (int i = 0; i < INITIAL_ROW; i++) {
			//find the cluster the wine is most closely related to, if not the cluster being checked, then skip it 
			double max = 0.0;
			int c = 0;
			for (int j = 0; j < clusters; j++) {
				if (max < membership_data[i].at(j)) {
					max = membership_data[i].at(j);
					c = j;
				}
			}
			//wine is most closely related to cluster being checked, perform tally up Manhattan version 
			if (c == cluster) {
				int tally = 0;
				for (int col = 0; col < INITIAL_COL; col++) {
					//add up all ones if they are at the same position 
					if (wines_data[i].at(col) == e[col])
						tally += e[col];
				}
				//if hit minimum value, then return first found wine as medoid/centroid 
				if (tally == min)
					return (wines_data[i]);
			}
		}
	}

	//If no wine found, return centroid from Manhattan Option #1 
	return (e);
}


//[KHB] write to file in FuzzyC
void FuzzyC::writeToFile(bool modulus) {
	if (modulus) {
		//For Modulus File
		//Open file for each iteration of mod
		ofstream output_file("../res/membershipdata_split" + to_string(mod + 1) + ".txt");
		ostream_iterator<double> output_iterator(output_file, ", ");
		//Write to file
		for (int i = 0; i < membership_data.size(); i++) {
			output_file << "Wine " + to_string(collapsed_wines_previndex[i]) + ", ";
			copy(membership_data.at(i).begin(), membership_data.at(i).end(), output_iterator);
			output_file << '\n';
		}

		//For removed attribute values
		ofstream output_file_removed_data("../res/membership_removed" + to_string(mod + 1) + ".txt");
		ostream_iterator<double> removed_iterator(output_file_removed_data, ", ");
		//Write to file
		for (int i = 0; i < removed_wines_data.size(); i++) {
			output_file_removed_data << "Wine " + to_string(removed_wines_previndex[i]) + ", ";
			copy(removed_wines_data.at(i).begin(), removed_wines_data.at(i).end(), removed_iterator);
			output_file_removed_data << '\n';
		}

		//For Test Wine Distances
		ofstream output_file_test_distance("../res/test_distance" + to_string(mod + 1) + ".txt");
		ostream_iterator<double> test_iterator(output_file_test_distance, ", ");
		//Write to file
		for (int i = 0; i < distance_data.size(); i++) {
			output_file_test_distance << "Wine " + to_string(removed_wines_previndex[i]) + ", ";
			copy(distance_data[i].begin(), distance_data[i].end(), test_iterator);
			output_file_test_distance << '\n';
		}
	}
	else {
		//Open file for each iteration of mod
		ofstream output_file("../res/membershipdata.txt");
		ostream_iterator<double> output_iterator(output_file, ", ");
		//Write to file
		for (int i = 0; i < membership_data.size(); i++) {
			output_file << "Wine " + to_string(collapsed_wines_previndex[i]) + ", ";
			copy(membership_data.at(i).begin(), membership_data.at(i).end(), output_iterator);
			output_file << '\n';
		}
	}
}

void FuzzyC::collapseData(bool option) {
	collapsed_wines_data.clear();
	collapsed_wines_previndex.clear();
	removed_wines_data.clear();
	removed_wines_previndex.clear();
	if (option) {
		for (int i = 0; i < wines_data.size(); i++) {
			if (i % fold != mod) {
				collapsed_wines_data.push_back(wines_data[i]);
				collapsed_wines_previndex.push_back(i);
			}
			else {
				removed_wines_data.push_back(wines_data[i]);
				removed_wines_previndex.push_back(i);
			}
		}
	}
	else {
		for (int i = 0; i < wines_data.size(); i++) {
				collapsed_wines_data.push_back(wines_data[i]);
				collapsed_wines_previndex.push_back(i);
		}
	}
	INITIAL_ROW = collapsed_wines_data.size();
}

void FuzzyC::run() {
		//Collapse Wines_Data based on modulus
		collapseData(false);
		//Generate random clusters based on current points
		initializeClusters();
		//Find the membership value for each wine to each cluster
		initializeMembership();
		//Generate new cluster centers based on membership values
		generateCenters();
		do {
			initializeMembership();
			generateCenters();
		} while (checkTermination());//Perform algorithm until convergence
		//[KHB] write to file for SVM  
		writeToFile(false);
}
//[KHB] Five Fold testing, run 'FOLD' times with different folds being used and omitted 
void FuzzyC::fiveFoldTest() {
	for (int i = 0; i < fold; i++) {
		//Collapse Wines_Data based on modulus
		collapseData(true);
		//Generate random clusters based on current points
		initializeClusters();
		//Find the membership value for each wine to each cluster
		initializeMembership();
		//Generate new cluster centers based on membership values
		generateCenters();
		do {
			initializeMembership();
			generateCenters();
		} while (checkTermination());//Perform algorithm until convergence
		//Calculate the distance of each Test/Removed Wine to the calculated centroids for SVM purposes
		calculateTest();
		//[KHB] write to file for SVM  
		writeToFile(true);
		//[KHB] move to next set of folds 
		mod++;
	}
}

//Calculate the distance of Test wines for SVM
vector<vector<double>> FuzzyC::calculateTest() {
	distance_data.clear();
	vector<double> element;
	for (int i = 0; i < removed_wines_data.size(); i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(jaccardDistance(i, cluster_points[j], true)); //Compare each wine to each other wine, find the distance
		}
		distance_data.push_back(element); //Store distance of each wine to wine index
		element.clear();
	}
	return distance_data;
}