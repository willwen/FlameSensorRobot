#ifndef InternalMap_h
#define InternalMap_h

#include "Arduino.h"

#define MAX_X 2
#define MAX_Y 2

const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;

//encode all visited states with 3 possibilties:
//forward, left and eight = 2^3 = 8 states
//const char NOT_VISITED = 'A';
//const char FORWARD_ONLY = 'B';
//const char LEFT_ONLY = 'C';
//const char RIGHT_ONLY = 'D';
//const char FORWARD_LEFT = 'E';
//const char FORWARD_RIGHT  = 'F';
//const char LEFT_RIGHT = 'G';
//const char ALL_VISITED = 'H';


//encode paths that are not allowed , given from ultrasonic sensor data
//like a wall in front, dont go forward

const char ALL_ALLOWED = 'A';
const char NO_FORWARD = 'B';
const char NO_LEFT = 'C';
const char NO_RIGHT = 'D';
const char NO_FORWARD_LEFT = 'E';
const char NO_FORWARD_RIGHT  = 'F';
const char NO_LEFT_RIGHT = 'G';
const char ALL_BLOCKED = 'H';

////encode outputs , forward, left, right, or full reverse
#define GO_FORWARD 1
#define GO_LEFT 2
#define GO_RIGHT 3
#define GO_REVERSE 4

// //column names
// #define A 0
// #define B 1
// #define C 2
// #define D 3
// #define E 4
// #define F 5
// #define G 6
// #define H 7



class InternalMap{
	public:
		//get a direction you have not gone before
		InternalMap();
//		int giveNewSafeSpot(int x , int y, char notAllowedPaths);
    void updateStep(int &x, int &y, int direction);
    
	private:
		//2 d array of positions
//		char map[MAX_Y][MAX_X];
//		char possibiltiesMap[8][8] = 
//		{
//			{'A','G','F','E','D','C','B','H'},
//			{'G','G','D','C','D','C','H','H'},
//			{'F','D','F','B','D','H','B','H'},
//			{'E','C','B','E','H','C','B','H'},
//			{'D','D','D','H','D','H','H','H'},
//			{'C','C','H','C','H','C','H','H'},
//			{'B','H','B','B','H','H','B','H'},
//			{'H','H','H','H','H','H','H','H'}
//		};
//		void updatePathTaken(int x , int y, int newPathTaken);
//		int giveDirectionFromSubsets(char encodedSubset);
//		int encodedValueToColumnNumber(char val);
};

#endif
