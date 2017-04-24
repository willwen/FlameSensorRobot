#include "InternalMap.h"

InternalMap::InternalMap(){

}
//given a spot and paths you cannot go, give me a spot I have not
//visited before
int InternalMap::giveNewSafeSpot(int x, int y, char notAllowedPaths){
	int directionToTake = giveDirectionFromSubsets(
		possibiltiesMap[
			encodedValueToColumnNumber(notAllowedPaths),
			map[y][x]]);
	updatePathTaken(x, y, directionToTake);
	return directionToTake;
}



void InternalMap::updatePathTaken(int x, int y, int newPathTaken){
	switch(map[y][x]){
		case ('A'): // none were visited
		{//now you visit newPathTaken
			if(newPathTaken == 1){ //if you went forward
				map[y][x] = 'B'; //left right remain
      break;
			}
		}
		case ('B'):
		{ //{F}
			if(newPathTaken == 1) //if you went forward
				map[y][x] = 'H'; //you visited them all
      break;
		}
		case ('C'):
		{// if you can only go left
			if(newPathTaken == 2) //and you go left
				map[y][x] = 'H';
      break;
		}
		case ('D'):
		{
			if(newPathTaken == 3)
				map[y][x] = 'H';
      break;
		}
		case ('E'):
		{ //{F, L}
			if(newPathTaken == 1)
				map[y][x] = 'C'; //only left left
      break;
		}
		case ('F'):
		{//{F,R}
			if(newPathTaken == 1)
				map[y][x] = 'D'; //only righ left
     break;
		}
		case ('G'):
		{
			if(newPathTaken == 2)// take left
				map[y][x] = 'D'; //only right left
      break;
		}
		case ('H'):
		{
			if(newPathTaken == 4)
				map[y][x] = 'H';
      break;
		}
	}
}

int giveDirectionFromSubsets(char encodedSubset){
	switch(encodedSubset){
		case ('A'):
			return 1;
		case ('B'):
			return 1;
		case ('C'):
			return 2;
		case ('D'):
			return 3;
		case ('E'):
			return 1;
		case ('F'):
			return 1;
		case ('G'):
			return 2;
		case ('H'):
			return 4;
	}
}

int encodedValueToColumnNumber(char val){
	switch(val){
		case ('A'):
			return 0;
		case ('B'):
			return 1;
		case ('C'):
			return 2;
		case ('D'):
			return 3;
		case ('E'):
			return 4;
		case ('F'):
			return 5;
		case ('G'):
			return 6;
		case ('H'):
			return 7;
	}
}

void updateStep(int &x, int &y, int direction){
  switch(direction){
    case NORTH:{
      y += 1;
    }
    case EAST:{
      x -= 1;
    }
    case SOUTH:{
      y -=1 ;
    }
    case WEST:{
      x += 1;
    }
  }
}

