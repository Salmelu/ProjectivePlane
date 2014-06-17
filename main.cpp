/* 
 * File:   main.cpp
 * Author: salmelu
 *
 * This program creates a graph representation of Projective planes.
 * It creates a dot-like output for creating a projective plane of n-th order.
 * The n must be a prime number.
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*
 *	Helping class representing a vector in 3D vector space.
 *  Implements dot product and holds a custom id for each vector which is 
 *  later used for the vertices of the resulting graph.
 */
class Vector {
public:

	Vector(int x1 = 0, int x2 = 0, int x3 = 0) : x1(x1), x2(x2), x3(x3) {
		this->id = 0;
	};

	Vector(int x1, int x2, int x3, int id) : x1(x1), x2(x2), x3(x3), id(id) {

	};

	int dotProduct(Vector* other) {
		return (this->x1 * other->getX(1) + this->x2 * other->getX(2) + this->x3 * other->getX(3));
	}

	int getX(int order) {
		switch (order) {
			case 1:
				return this->x1;
			case 2:
				return this->x2;
			case 3:
				return this->x3;
		}
		return 0;
	}

	int getId() {
		return this->id;
	}
private:
	int x1, x2, x3;
	int id;
};

/*
 * Helpful function checking whether user entered a prime number.
 */
bool isPrime(int p) {
	for (int i = 2; i <= sqrt(p); i++) {
		if (p % i == 0)
			return false;
	}
	return true;
}

/*
 * Function cycling a pool of colors and returning a color.
 * Used for graph edges (lines).
 */
string getColor(int i) {
	switch (i % 13) {
		case 0:
			return "red";
		case 1:
			return "blue";
		case 2:
			return "green";
		case 3:
			return "orange";
		case 4:
			return "gray";
		case 5:
			return "purple";
		case 6:
			return "cyan";
		case 7:
			return "brown";
		case 8:
			return "chocolate4";
		case 9:
			return "crimson";
		case 10:
			return "goldenrod";
		case 11:
			return "indigo";
		case 12:
			return "navyblue";
		default:
			return "black";
	}
}

/*
 * Function constructing an array of vertices represented by a characteristic
 * vector.
 * Takes order as its parameter.
 */
Vector** constructVertices(int o) {
	int vsize = (o * o) + o + 1;
	Vector** vertices = new Vector*[vsize]; // Create an empty array

	int index = 1;
	vertices[0] = new Vector(1, 0, 0, 0);	// First vector is trivial

	for (int a = 0; a < o; index++, a++) {	// Vectors of type (a, 1, 0)
		vertices[index] = new Vector(a, 1, 0, index);
	}

	for (int b = 0; b < o; b++) {			// Vectors of type (b, a, 1)
		for (int a = 0; a < o; index++, a++) {
			vertices[index] = new Vector(b, a, 1, index);
		}
	}
	
	return vertices;
}

/*
 *	A function constructing lines of a projective plane.
 *	Takes an array of vertices as its first parameter, a 3D array of booleans
 *	representing similar vectors and an order as its third parameter.
 *	Returns an array of lines, each line being an array of vertices it lays on.
 */
Vector*** constructLines(Vector** vertices, bool* unique, int o) {
	Vector*** lines = new Vector**[o * o * o];	// Construct an array of lines

	for (int a = 0; a < o; a++) {
		for (int b = 0; b < o; b++) {
			for (int c = 0; c < o; c++) {		// For all vectors (a, b, c)
				if (!unique[(a * o * o) + (b * o) + c]) continue;	
					// The vector has been already examined

				Vector** line = new Vector*[o + 1];	// Create a new line
				for (int i = 0, j = 0; j <= o; i++) {	// Find its vertices
					if ((vertices[i]->dotProduct(new Vector(a, b, c)) % o) == 0) {
						line[j] = vertices[i];
						j++;
					}
				}

				lines[a * o * o + b * o + c] = line;	// Add the line
				for (int i = 2; i < o; i++) {			
					// Set false to all multiples of the current vector to avoid
					// multiple same lines.
					unique[((i * a) % o) * o * o + ((i * b) % o) * o + (i * c) % o] = false;
				}
			}
		}
	}

	return lines;
}

int main(int argc, char** argv) {

	int order;
	string filename;

	cout << "Enter the order of your desired projective plane (a prime): \n";
	cin >> order;
	if (order < 2) {
		cout << "There is no projective plane that small. \n";
		return 1;
	}
	if (!isPrime(order)) {
		cout << "The entered number is not a prime. \n";
		return 1;
	}
	if (order > 100) {
		cout << "This projective plane would kill your computer. Don't even try it. \n";
		return 1;
	}

	cout << "Enter the filename where the graph representation should be saved: \n";
	cin >> filename;
	ofstream f;
	f.open (filename.c_str(), std::ofstream::out);
	if(!f.is_open()) {
		cout << "Some error happened. Couldn't open the file for writing. \n";
		return 2;
	}
	
	Vector** vertices = constructVertices(order); // Create the vertices

	bool* unique = new bool[order * order * order]; // Create the unique array
	unique[0] = false;		// Vector (0,0,0) should not be tested
	for (int i = 1; i < order * order * order; i++)
		unique[i] = true;	// Others should

	// Construct the lines
	Vector*** lines = constructLines(vertices, unique, order); 

	// Create the dot representation
	f << "Graph K {" << endl;	
	for (int i = 0; i < (order * order + order + 1); i++) // State the vertices
		f << "\t" << i << endl;
	f << endl;
	int p = 0;
	for (int i = 1; i < (order * order * order); i++) {	// Cycle through lines
		if (!unique[i]) continue;	// That would be a copied line
		f << "\t";
		for (int j = 0; j < order + 1; j++) {
			f << "" << lines[i][j]->getId();
			if (j < order) f << " -- ";
		}
		// Allows to use different colors for the lines
		f << " [color = " << getColor(p++) << "]";	
		f << endl;
	}
	f << "}" << endl;
	f.close();
	
	cout << "Graph was created and saved in file " << filename << "." <<endl;
	// No frees needed, the program ends...
	return 0;
}

