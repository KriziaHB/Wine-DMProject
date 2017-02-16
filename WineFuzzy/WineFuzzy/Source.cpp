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
using namespace std;

string wineReviews[1011][50]; //Name, Vintage, Grade, Review, Author, Attributes 1 through n, Price, Country, Region, IssueDate 
string wines[1011][306]; //KT: 11*26 + 20; Wine name then attributes 


void readFromFile() {
	//full_data.txt & wines.csv
	//C:/Users/buckkr/Source/Repos/Wine-DMProject
	//ifstream reviews("../full_data.txt", ios::in);
	//fstream wineCSV("../wines.csv", ios::in); 
	//Add your reviews and wineCSV files below in this format and comment out all others when running on your machine 

	//KRIZIA 
	ifstream reviews("C:/Users/buckkr/Source/ReposWine-DMProject/full_data.txt", ios::in);
	fstream wineCSV("C:/Users/buckkr/Source/Repos/Wine-DMProject/wines.csv", ios::in);

	//CALEB 


	//CHRIS 


	//JEREMY 


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

	// formula 
	//Simplified by adding together both cases of 1 vs 0 when comparing wines 
	dist = double(a / (a + b)); 
	return(dist);
}


void main() {

	cout << "**In main" << endl;

	//read in files to 2D arrays 
	readFromFile();
	cout << "**Back in main after readFromFile executed" << endl;

	//Test Jaccard's 
	double dist1 = JaccardDistance(1, 2); 
	double dist2 = JaccardDistance(2, 3); 
	cout << "Wine 1 vs Wine 2: " << dist1 << endl; 
	cout << "Wine 2 vs Wine 3: " << dist2 << endl; 
	double dist3 = JaccardDistance(1, 2);
	double dist4 = JaccardDistance(2, 3);
	cout << "Wine 3 vs Wine 4: " << dist3 << endl;
	cout << "Wine 4 vs Wine 5: " << dist4 << endl;



	//Wait to terminate 
	cout << "Terminate the program";
	getchar();
}