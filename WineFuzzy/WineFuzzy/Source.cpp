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
#include "FuzzyC.h"
using namespace std;

string wineReviews[1011][50]; //Name, Vintage, Grade, Review, Author, Attributes 1 through n, Price, Country, Region, IssueDate 
string wines[1011][306]; //KT: 11*26 + 20; Wine name then attributes 
#define RGB_COMPONENT_COLOR 255

const int X_COL = 1011;
const int Y_COL = 1011;

void readFromFile() {
	//full_data.txt & wines.csv
	//C:/Users/buckkr/Source/Repos/Wine-DMProject
	ifstream reviews("../res/full_data.txt", ios::in);
	fstream wineCSV("../res/wines.csv", ios::in);


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
					while (getline(stream, section, ','))
					{
						if (section.at(0) == '|')
							break;
						wineReviews[i][j] = section;
						j++;
					}
					counter = 0;
					getline(stream, section, '|');
				}
			}
			counter = 0;
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


void main() {
	//read in files to 2D arrays 
	readFromFile();

  cout << "**In main" << endl;
	readFromFile();
	FuzzyC fuzzy(2, wines);

	//Test Jaccard's 
	double dist1 = fuzzy.JaccardDistance(1, 2);
	double dist2 = fuzzy.JaccardDistance(2, 3);
	cout << "Wine 1 vs Wine 2: " << dist1 << endl;
	cout << "Wine 2 vs Wine 3: " << dist2 << endl;
	double dist3 = fuzzy.JaccardDistance(3, 4);
	double dist4 = fuzzy.JaccardDistance(4, 5);
	cout << "Wine 3 vs Wine 4: " << dist3 << endl;
	cout << "Wine 4 vs Wine 5: " << dist4 << endl;

	//Visual representation of the original 2D array (matrix) of wines and their attributes 
	writePGM("../res/WINEmatrix.pgm", 1011, 306);
//	writePGM("C:/Users/buckkr/Desktop/WINEmatrix.pgm", 1011, 306); 

	//Reorder wine matrix to show the clusters better then write visual representation after row reordering

	writePGM("../res/CLUSTERmatrix.pgm", 1011, 306); 


	//Wait to terminate 
	cout << "Terminate the program";
	getchar();
}