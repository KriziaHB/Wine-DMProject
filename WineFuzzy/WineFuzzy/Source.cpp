//Krizia Houston Buck 
//Fuzzy C-Means Clustering with Jaccard's Distance Formula 
//02/22/2017 

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
#include "FuzzyC.h" 
using namespace std;

string wineReviews[1011][50]; //Name, Vintage, Grade, Review, Author, Attributes 1 through n, Price, Country, Region, IssueDate 
string wines[1011][306]; //KT: 11*26 + 20; Wine name then attributes 
#define RGB_COMPONENT_COLOR 255


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


//Old Jaccard
/*
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
*/ 



void writePGM(const char *filename, int dim1, int dim2) //KHB change to work with grayscale & 2D arrays 
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
	
	
	fclose(fp);
}


void writeMemberships(const char *filename, int dim1, int dim2, vector<vector<double>> membership) {
	fstream fp;
	//open file for output
	fp.open(filename); 

	//header 
	fp << "Wines"; 
	//Cluster numbers 
	for (int i = 1; i <= dim2; i++) {
		fp << ", " << i; 
	}
	fp << "\n"; 

	//Membership data 
	for (int i = 0; i < dim1; i++) {
		fp << "Wine " << i + 1; 
		for (int j = 0; j < dim2; j++) {
			fp << ", " << membership[i].at(j); 
		}
		fp << " \n"; 
	}

	//close the text file 
	fp.close(); 
}


void main() {
	//read in files to 2D arrays 
	readFromFile();

	//User defined k (number of clusters) 
	int k; 
	cout << "Please enter an integer value for the number of clusters 'k': "; 
	cin >> k; 
	k = int(k); 
	if (k < 2)
		k = 2; 
	//Call to start up Fuzzy C  
	cout << "k value: " << k << endl; 
	FuzzyC fuzzy(k, 2, wines);

/*
	//Test Jaccard's 
	double dist1 = fuzzy.jaccardDistance(1, 2);
	double dist2 = fuzzy.jaccardDistance(2, 3);
	cout << "Wine 1 vs Wine 2: " << dist1 << endl;
	cout << "Wine 2 vs Wine 3: " << dist2 << endl;
	double dist3 = fuzzy.jaccardDistance(3, 4);
	double dist4 = fuzzy.jaccardDistance(4, 5);
	cout << "Wine 3 vs Wine 4: " << dist3 << endl;
	cout << "Wine 4 vs Wine 5: " << dist4 << endl;
*/

	//Gather membership data for printing to text file for SVM analysis 
	vector<vector<double>> md = fuzzy.printMD();

	//Visual representation of the original 2D array (matrix) of wines and their attributes 
	writePGM("../res/WINEmatrix.pgm", 1011, 306);
//	writeMemberships("../res/MembershipData.txt", 1011, k, md); 

	//Wait to terminate 
	cout << "Terminate the program";
	getchar();
}