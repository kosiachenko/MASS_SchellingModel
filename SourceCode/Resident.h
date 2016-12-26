#ifndef Resident_H
#define Resident_H

#include "Agent.h"
#include "MASS_base.h"
#include <sstream> // ostringstream
using namespace std;

//For debug printing:
const bool printOut = false;

class Resident : public Agent {
 public:
  // define functionId's that will 'point' to the functions they represent.
  static const int init_ = 0;
  static const int communicateToHouse_ = 2;
  static const int calculateSatisfaction_ = 3;
  static const int changeHome_ = 4;
  static const int checkIfPrimaryRes_ = 5;
  static const int provideID_ = 6;
  static const int resetForNewTurn_ = 7;

  //static const int cardinals[25][2];
  
   /**
   * Initialize a Resident object
   */
  Resident( void *argument ) : Agent( argument ) {
    if(printOut == true)
        MASS_base::log( "BORN!!" );
  };
  
  /**
   * the callMethod uses the function ID to determine which method to execute.
   * It is assumed the arguments passed in contain everything those 
   * methods need to run.
   */
  virtual void *callMethod( int functionId, void *argument ) {
    switch( functionId ) {
    	case init_: return agentInit( argument );
      case communicateToHouse_: return communicateToHouse();
      case calculateSatisfaction_: return calculateSatisfaction();
      case changeHome_: return changeHome(argument);
      case checkIfPrimaryRes_: return checkIfPrimaryRes();
      case provideID_: return provideID();
      case resetForNewTurn_: return resetForNewTurn(argument);

    }

    return NULL;
  };
  
 virtual int map( int initPopulation, vector<int> size, vector<int> index, Place* curPlace) {
    int ratio = 100*initPopulation / (size[0]*size[1]);  //percentage of non-empty cells
    int randNumber = rand()%100; //random number from 0 to 100

    if (randNumber <= ratio) {
        return 1;
    }
    else {
        return 0;
    }
 }

 private:
  int status;  //1 - blue, 2 - red
  bool satisfied;  //if true - stays in place, false - moves to another cell
  bool primaryResident; //if true - primary resident of the house, if false - has to move
  int threshold;

  //Functions to be called by callAll():
  void *agentInit( void *argument );
  void *communicateToHouse();
  void *calculateSatisfaction();
  void *changeHome(void *argument);
  void *checkIfPrimaryRes();
  void *provideID();
  void *resetForNewTurn(void *argument );

};

#endif
