//Krizia Houston Buck 
//Fuzzy C-Means Clustering with Jaccard's Distance Formula 
//02/21/2017 

#include <iostream> 
#include <iomanip> 
#include <array> 
#include <fstream> 
#include <sstream> 
#include <string> 
#include <stdlib.h>
#include <stdio.h>
#include <cstdio> 
#include <vector>
using namespace std;

string wineReviews[1011][50]; //Name, Vintage, Grade, Review, Author, Attributes 1 through n, Price, Country, Region, IssueDate 
string wines[1011][306]; //KT: 11*26 + 20; Wine name then attributes 
string sortedWines[1011][306]; //KHB: use for printing out the final image after wines are reordered in their clusters 
#define RGB_COMPONENT_COLOR 255


//CHRIS 
int clusters;
int m; 
vector<int> cluster_points;
vector<vector<double>> distance_data;
vector<vector<string>> wines_data;
vector<vector<double>> membership_data;

void readFromFile() {
	//full_data.txt & wines.csv
	//C:/Users/buckkr/Source/Repos/Wine-DMProject
	ifstream reviews("../res/full_data.txt", ios::in);
	fstream wineCSV("../res/wines.csv", ios::in);
	//KRIZIA 
	//ifstream reviews("C:/Users/buckkr/Source/ReposWine-DMProject/full_data.txt", ios::in);
	//fstream wineCSV("C:/Users/buckkr/Source/Repos/Wine-DMProject/wines.csv", ios::in);

	//read to wineReviews[][] 
	if (reviews.is_open()) {
		string line;
		int i = 0;
		int j = 0;
		int counter = 0;

		//iterate through text file lines 
		while (getline(reviews, line)) {
			string section;
			stringstream stream(line);
			//iterate through sections within each line 
			while (getline(stream, section, '|')) {
				wineReviews[i][j] = section;
				j++;
				counter++;
				if (counter == 5)
				{
					while (getline(stream, section, ','))			//start seperating out the attributes
					{
						if (section.at(0) == '|')				//if the attributes section is finished
						{
							counter = 0;
							stringstream stream2(section);
							string section2;
							while (counter<5) //run until the line is finished
							{
								getline(stream2, section2, '|');
								if (counter == 0)
								{
									counter++;
								}
								else
								{
									wineReviews[i][j] = section2;
									j++;
									counter++;
								}
							}
							break;
						}
						else
						{
							wineReviews[i][j] = section;
							j++;
						}
					}
					while (getline(stream, section, '|')) //if before the attributes
					{
						wineReviews[i][j] = section;
						j++;
					}
					counter = 0;
				}
			}
			j = 0;
			i++;
		}
		//Cleanup 
		reviews.close();
	}
	else
		cout << "Unable to open the Wine Reviews text file" << endl;

	//read to wines[][] 
	if (wineCSV.is_open()) {
		string line;
		int i = 0;
		int j = 0;
		int counter = 0;

		//iterate through CSV file lines 
		while (getline(wineCSV, line)) {
			string section;
			stringstream stream(line);
			//iterate through sections within each line 
			while (getline(stream, section, ',')) {
				counter++; 
				//go down to next row 
				if (counter == 306) {
					counter = 0;
					i++;
					j = 0;
					wines[i][j] = section; 
//					cout << "wines[" << i << "][" << j << "]: " << wines[i][j] << ";" << endl;
				}
				else {
					wines[i][j] = section;
//					cout << "wines[" << i << "][" << j << "]: " << wines[i][j] << ";" << endl;
					j++;
				}
			}
		}
//		cout << "i: " << i << endl;
//		cout << "j: " << j << endl;
//		cout << "Counter: " << counter << endl;

		//Cleanup 
		wineCSV.close();
	}
	else
		cout << "Unable to open the Wine excel file" << endl;
}


double JaccardDistance(int x1, int x2) {
	double a = 0.0; //# of attributes of A is 0 while 1 in B && # of attributes of A is 1 while 0 in B 
	double b = 0.0; //# of attributes where A and B have value of 1 
	double dist = 0.0;

	// count up a (different values) and b (both have 1) between the two points 
	for (int i = 1; i < 306; i++) {
		if ((wines[x1][i] == "1") && (wines[x2][i] == "1"))
			b++; 
		else if (wines[x1][i] != wines[x2][i])
			a++; 
	}

	//Formula: simplified by adding together both cases of 1 vs 0 when comparing wines 
	dist = double(a / (a + b)); 
	return(dist);
}




//ADDING CHRIS'S FUNCTIONS 
void initializeMembership() { //determine membership of each wine to a cluster 
	//Generate Jaccard's distance
	FeedData();
	//Clear old membership data (This is for calculating new values)
	membership_data.clear();
	vector<double> element;
	for (int i = 0; i < distance_data.size(); i++) {
		for (int j = 0; j < cluster_points.size(); j++) {
			element.push_back(CalculateMembership(i, cluster_points[j])); //Calculate membership value for wine to each cluster point
		}
		membership_data.push_back(element);//Collect both cluster points for single wine and loop
		element.clear();
	}
}


void generateCenters() { //recalculate centroid (when calculations don't change much then the program can stop) 
	vector<string> element;
	for (int i = 0; i < cluster_points.size(); i++) {
		for (int j = 0; j < 304; j++) {
			element.push_back(to_string(calculateCentroid(j, i)));//For each attribute calculate the centroid (or average of all points in attribute)
		}
		cluster_points[i] = wines_data.size(); //Assign new index as the new cluster point
		wines_data.push_back(element); //Append new centroid to wine data
		element.clear();
	}
}

double calculateCentroid(int col, int cluster) {
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

double CalculateMembership(int wine, int cluster) {
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
		value += pow((distance_to_cluster / distance_data[wine].at(cluster_points[i])), power_value);//summation of all distance to cluster points
	}
	value = 1 / value;

	return value;
}


void initializeClusters(int clusters) {
	//select random wines as the centers 
	for (int i = 0; i < clusters; i++) {
		//Centroids of each cluster 
		cluster_points.push_back(rand() % 1011 + 1);
	}
	//for test purposes 
	cluster_points[1] = 20; 
}

void FeedData() {
	vector<double> element;

	//Test with i and j as 1 going to 10 for a small sample size for now 
	for (int i = 1; i < 50; i++) {
		for (int j = 1; j < 50; j++) { //Eventually, j needs to be j = i + 1 so that we don't fill up the entire matrix 
			element.push_back(JaccardDistance(i, j));
		}
		//distance_data is a vector of Jaccard's Distances between wines 
		distance_data.push_back(element);
		element.clear();
	}
//	return distance_data;
}


void FuzzyC(int clusters, int m_value) //, string wines[1011][306])
{
	//Element is a row of wine data 
	vector<string> element;
	//copy wine data over - trimmed 
	for (int i = 1; i < 1010; i++) {
		for (int j = 1; j < 305; j++) {
			element.push_back(wines[i][j]);
		}
		//wines_data is all of the rows with the columns from element 
		wines_data.push_back(element);
		element.clear();
	}
	//sets the m value for fuzzyness
	m = m_value; 
	//generate random clusters based on current points 
	initializeClusters(clusters);
	//find the membership value for each wine to each cluster 
	initializeMembership(); 
	//generate new cluster centers based on membership values 
	generateCenters(); 
}
//END CHRIS'S FUNCTIONS 



void writePGM(const char *filename, int dim1, int dim2, int type) //KHB change to work with grayscale & 2D arrays 
{
	FILE *fp;
	//open file for output
	fopen_s(&fp, filename, "wb");//writing in binary format
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//write the header file
	//image format
	fprintf(fp, "P5\n");
	//image size
	fprintf(fp, "%d %d\n", dim2, dim1);
	// rgb component depth
	fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

	//Write from original wine data 
	if (type == 1)
	{
		//Write to file  
		for (int row = 0; row < dim1; row++)
		{
			for (int col = 0; col < dim2; col++)
			{
				int pix = 255;
				if ((row == 0) || (col == 0))
					fputc(char(pix), fp);
				//Black (0) if has the attribute, white (255) if does not 
				else if (wines[row][col] == "1") {
					pix = 0;
					fputc(char(pix), fp);
				}
				else {
					pix = RGB_COMPONENT_COLOR;
					fputc(char(pix), fp);
				}
			}
		}
	}
	//Write from newly sorted and clustered wine data 
	else
	{
		//Write to file  
		for (int row = 0; row < dim1; row++)
		{
			for (int col = 0; col < dim2; col++)
			{
				int pix = 255;
				if ((row == 0) || (col == 0))
					fputc(char(pix), fp);
				//Black (0) if has the attribute, white (255) if does not 
				else if (sortedWines[row][col] == "1") {
					pix = 0;
					fputc(char(pix), fp);
				}
				else {
					pix = RGB_COMPONENT_COLOR;
					fputc(char(pix), fp);
				}
			}
		}
	}
	fclose(fp);
}


void main() {
	//read in files to 2D arrays 
	readFromFile();

	/*
	//Test Jaccard's 
	double dist1 = JaccardDistance(1, 2); 
	double dist2 = JaccardDistance(2, 3); 
	cout << "Wine 1 vs Wine 2: " << dist1 << endl; 
	cout << "Wine 2 vs Wine 3: " << dist2 << endl; 
	double dist3 = JaccardDistance(3, 4);
	double dist4 = JaccardDistance(4, 5);
	cout << "Wine 3 vs Wine 4: " << dist3 << endl;
	cout << "Wine 4 vs Wine 5: " << dist4 << endl; */


	//User defined k (number of clusters) 
	int k; 
	cout << "Please enter an integer value for the number of clusters 'k': "; 
	cin >> k; 
	k = int(k); 
	if (k < 2)
		k = 2; 
	//Call to start up Fuzzy C  
	cout << "k value: " << k << endl; 
	FuzzyC(k, 2); 
	

	//Visual representation of the original 2D array (matrix) of wines and their attributes 
	writePGM("../res/WINEmatrix.pgm", 1011, 306, 1);

	//Reorder wine matrix to show the clusters better then write visual representation after row reordering

	writePGM("../res/CLUSTERmatrix.pgm", 1011, 306, 2); 

	//Wait to terminate 
	cout << "Terminate the program";
	getchar();
}