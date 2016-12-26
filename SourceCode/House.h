#ifndef House_H
#define House_H

#include <string.h>
#include "Place.h"

using namespace std;

class House : public Place {
public:

	static const int NUM_NEIGHBORS = 8;									//A House neighbors
  static const int NUM_EXTENDED_NEIHBORS = 48;
  
  // define functionId's that will 'point' to the functions they represent.
  static const int init_ = 0;
  static const int printCurrentState_ = 1;
  static const int setOccupancy_ = 2;
  static const int displayStatusAsOut_ = 3;
  static const int receiveNeigborStatus_ = 4;
  static const int getSimilarityPerc_ = 5;
  static const int getHouseReservation_ = 6;
  static const int resetForNewTurn_ = 7;
  static const int getPrimaryResident_ = 8;


  /**
   * Initialize a House object by allocating memory for it.
   */
  House( void *argument ) : Place( argument ) {
    bzero( arg, sizeof( arg ) );
    strcpy( arg, (char *)argument );
  };
  
  /**
   * the callMethod uses the function ID to determine which method to execute.
   * It is assumed the arguments passed in contain everything those 
   * methods need to run.
   */
  virtual void *callMethod( int functionId, void *argument ) {
    switch( functionId ) {
      case init_: return init( argument );
      case printCurrentState_: return printCurrentState(argument);
      case setOccupancy_: return setOccupancy(argument);
      case displayStatusAsOut_: return displayStatusAsOut();
      case receiveNeigborStatus_: return receiveNeigborStatus();
      case getSimilarityPerc_: return getSimilarityPerc();
      case getHouseReservation_: return getHouseReservation();
      case resetForNewTurn_:return resetForNewTurn();
      case getPrimaryResident_: return getPrimaryResident();


    }
    return NULL;
  };

private:
    int occupantStatus;  //0 when empty, 1 - blue, 2 - red
    static int neighborCoord[8][2];   //Array of neighbor coordinates
    
    int extendedNeighbors[48][2];   //To look for empty cells to move
    int neighborStatus[8];  //An array to store status of neighbors
    
    int x_size;  //size of the matrix
    int y_size;

    char arg[100];

    //Functions to be called by callAll():
    void *init( void *argument );
    void *printCurrentState(void *argument);  
    void *setOccupancy(void *argument);
    void *displayStatusAsOut();  
    void *receiveNeigborStatus(); 
    void *getSimilarityPerc();
    void *getHouseReservation();
    void *resetForNewTurn();
    void *getPrimaryResident();
    
    //Support functions
    void *getEmptyNeighbors();
};

#endif
