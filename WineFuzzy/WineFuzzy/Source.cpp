//Krizia Houston Buck 
//Fuzzy C-Means Clustering with Jaccard's Distance Formula 
//02/13/2017 

#include <iostream> 
#include <iomanip> 
#include <array> 
#include <fstream> 
#include <sstream> 
#include <string> 
#include <stdio.h>
#include <vector>
#include "FuzzyC.h"
using namespace std;

string wineReviews[1011][50]; //Name, Vintage, Grade, Review, Author, Attributes 1 through n, Price, Country, Region, IssueDate 
string wines[1011][306]; //KT: 11*26 + 20; Wine name then attributes 

const int X_COL = 1011;
const int Y_COL = 1011;

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
				int check = section.find("\n");
				if (check > 1) {
					counter++;
					i++;
					j = 0;
				}
				else { //???? how to separate out each attribute that is in all caps? 
					wineReviews[i][j] = section;
					//				cout << wineReviews[i][j] << endl;
					j++;
				}
			}
		}
		cout << "i: " << i << endl;
		cout << "j: " << j << endl;
		cout << "Counter: " << counter << endl;

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

		//iterate through text file lines 
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
					//				cout << wines[i][j] << endl;
				}
				else {
					wines[i][j] = section;
					//				cout << wines[i][j] << endl;
					j++;
				}
			}
		}
		cout << "i: " << i << endl;
		cout << "j: " << j << endl;
		cout << "Counter: " << counter << endl;

		//Cleanup 
		wineCSV.close();
	}
	else
		cout << "Unable to open the Wine excel file" << endl;
}


void main() {

	cout << "**In main" << endl;
	FuzzyC fuzzy(2, wines);
										  //read in files to 2D arrays 
	readFromFile();
	cout << "**Back in main after readFromFile executed" << endl;

	//Test Jaccard's 
	double dist1 = fuzzy.JaccardDistance(1, 2);
	double dist2 = fuzzy.JaccardDistance(2, 3);
	cout << "Wine 1 vs Wine 2: " << dist1 << endl;
	cout << "Wine 2 vs Wine 3: " << dist2 << endl;
	double dist3 = fuzzy.JaccardDistance(3, 4);
	double dist4 = fuzzy.JaccardDistance(4, 5);
	cout << "Wine 3 vs Wine 4: " << dist3 << endl;
	cout << "Wine 4 vs Wine 5: " << dist4 << endl;

	//Wait to terminate 
	cout << "Terminate the program";
	getchar();
}