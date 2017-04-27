#include "InternalMap.h"

InternalMap::InternalMap(){

}

void InternalMap::updateStep(int &x, int &y, int direction){
  Serial.println("x before:");
  Serial.println(x);
  Serial.println("y before:");
  Serial.println(y);
  Serial.println("direction:");
  Serial.println(direction);
  
  
  switch(direction){
    case NORTH:{
      y +=1;
      break;
    }
    case EAST:{
      x -=1;
      break;
    }
    case SOUTH:{
      y -=1 ;
      break;
    }
    case WEST:{
      x += 1;
      break;
    }
  }
  Serial.println("x after:");
  Serial.println(x);
  Serial.println("y after:");
  Serial.println(y);
}
//given a spot and paths you cannot go, give me a spot I have not
//visited before
//int InternalMap::giveNewSafeSpot(int x, int y, char notAllowedPaths){
//  Serial.println("x");
//  Serial.println(x);
//  Serial.println("y");
//  Serial.println(y);
//  Serial.println("not allowed paths");
//  Serial.println(notAllowedPaths);
//  if(map[y][x]=='\0'){
//    Serial.println("null val");
//    map[y][x]='A';
//  }
//    
//  Serial.println("map[y][x]");
//  Serial.println(map[y][x]);
//  Serial.println("val of map[y][x]");
//  Serial.println(encodedValueToColumnNumber(map[y][x]));
//  Serial.println("val of notAllowedPaths");
//  Serial.println(encodedValueToColumnNumber(notAllowedPaths));
//  Serial.println("possibilties val");
//  char c = possibiltiesMap[
//     encodedValueToColumnNumber(notAllowedPaths)][
//      encodedValueToColumnNumber(map[y][x])
//      ];
//  Serial.println(c);
//	int directionToTake = giveDirectionFromSubsets(c);
//	updatePathTaken(x, y, directionToTake);
//	return directionToTake;
//}


//
//void InternalMap::updatePathTaken(int x, int y, int newPathTaken){
//	switch(map[y][x]){
//		case ('A'): // none were visited
//		{//now you visit newPathTaken
//			if(newPathTaken == 1){ //if you went forward
//				map[y][x] = 'B'; //left right remain
//      
//			}
//      else if(newPathTaken == 2){ //if you went left
//        map[y][x] = 'C'; //forward right remain
//      }		
//      else if (newPathTaken == 3){
//        map[y][x] = 'D';
//      }
//      break;
//    }
//		case ('B'): //if you have gone forward before
//		{ //{F}
//			if(newPathTaken == 2) //this time you went left
//				map[y][x] = 'E'; //you have now gone forward and left
//      else if (newPathTaken == 3)
//        map[y][x] = 'F';
//      break;
//		}
//		case ('C'):
//		{// if you have gone left
//			if(newPathTaken == 1) //and you go forward
//				map[y][x] = 'E';
//      else if(newPathTaken == 3) // and now you go right
//        map[y][x] = 'G';
//      break;
//		}
//		case ('D'): //if you have gone right
//		{
//			if(newPathTaken == 1) // and now you go forward
//				map[y][x] = 'F';
//      else if (newPathTaken == 2) //and now you go left
//        map[y][x] = 'G';
//      break;
//		}
//		case ('E'):
//		{ //{F, L}
//			if(newPathTaken == 1)
//				map[y][x] = 'H'; //only right left
//      break;
//		}
//		case ('F'):
//		{//{F,R}
//			if(newPathTaken == 1)
//				map[y][x] = 'H'; //only left left
//     break;
//		}
//		case ('G'):
//		{
//			if(newPathTaken == 1)// take forward
//				map[y][x] = 'H'; 
//      break;
//		}
//		case ('H'):
//		{
//			if(newPathTaken == 4)
//				map[y][x] = 'H';
//      break;
//		}
//	}
//  Serial.println("Updated map[y][x] to val:");
//  Serial.println(map[y][x]);
//}
//
//int InternalMap::giveDirectionFromSubsets(char encodedSubset){
//  Serial.println("encoded char:");
//  Serial.println(encodedSubset);
//	switch(encodedSubset){
//		case ('A'):
//			return 1;
//		case ('B'):
//			return 1;
//		case ('C'):
//			return 2;
//		case ('D'):
//			return 3;
//		case ('E'):
//			return 1;
//		case ('F'):
//			return 1;
//		case ('G'):
//			return 2;
//		case ('H'):
//			return 4;
//	}
//}
//
//int InternalMap::encodedValueToColumnNumber(char val){
//	switch(val){
//		case ('A'):
//			return 0;
//		case ('B'):
//			return 1;
//		case ('C'):
//			return 2;
//		case ('D'):
//			return 3;
//		case ('E'):
//			return 4;
//		case ('F'):
//			return 5;
//		case ('G'):
//			return 6;
//		case ('H'):
//			return 7;
//	}
//}



/**
 * //        if (leftDist < SIDE_LAZER_THRESHOLD) {
//          Serial.println("Because of left sensor obstacle");
//          
//          if(notAllowedPaths == 'B'){
//            notAllowedPaths = 'E';
//          }
//          else{
//            notAllowedPaths = 'C';
//          }
//          Serial.println(notAllowedPaths);
//
//        }
//        else if (rightDist < SIDE_LAZER_THRESHOLD) {
//            Serial.println("Because of left sensor obstacle");
//
//            if (notAllowedPaths == 'A') // all allowed
//              notAllowedPaths = 'D';
//            else if(notAllowedPaths == 'B')
//              notAllowedPaths = 'F';
//            else if (notAllowedPaths == 'C')
//              notAllowedPaths = 'G';
//            else if (notAllowedPaths == 'E')
//              notAllowedPaths = 'H';
//            Serial.println(notAllowedPaths);
//        }

        //see if left or right sensor detect an obstacle
//        switch(internalMap.giveNewSafeSpot(x, y, notAllowedPaths)){
//          case (GO_FORWARD):{
//            state = 0;
//            break;
//          }
//          case (GO_LEFT):{
//            state = ROTATE_90_LEFT_STATE;
//            break;
//          }
//          case (GO_RIGHT):{
//            //rotate right 90
//            state = ROTATE_90_RIGHT_STATE;
//            break;
//          }
//          case(GO_REVERSE):{
//            //need to code state
//            break;
//          }
//         }

//        Serial.println("leftDist:");
//        Serial.println(leftDist);
//        Serial.println("rightDist:");
//        Serial.println(rightDist);
**/


