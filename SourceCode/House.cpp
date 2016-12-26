#include "House.h"
#include "MASS_base.h"
#include <sstream>     // ostringstream

const bool printOutput = true;

//Should include according to MASS manual:
extern "C" Place* instantiate( void *argument ) {
  return new House( argument );
}

extern "C" void destroy( Place *object ) {
  delete object;
}

int House::neighborCoord[8][2] = {{-1,-1}, {-1,0}, {-1,1},
                                  {0,-1},          {0,1},
                                  {1,-1},  {1,0}, {1,1}};

void *House::init( void *argument ) {
    resetForNewTurn();

    vector<int> matrixSize = getSizeVect();
    x_size = matrixSize[0];  //size of the matrix
    y_size = matrixSize[1];

    //fill an array of relative coordinates of extended neihborhood
    int pos = 0;
    for (int i = -3; i<=3; i++) {
        for (int j = -3; j<=3; j++) {
            if (!((i==0)&&(j==0))) {
                extendedNeighbors[pos][0] = i;
                extendedNeighbors[pos][1] = j;
                pos++;
            }   
        }
    }

    return NULL;
}

void *House::resetForNewTurn() {
    occupantStatus = 0; //empty

    for (int i = 0; i<NUM_NEIGHBORS; i++) {
        neighborStatus[i] = 0;  //empty
    }

    outMessage = NULL;
    outMessage_size = 0;

    return NULL;
}


void *House::setOccupancy(void *argument) {
    if(agents.size() == 0)
        return NULL;
    
    occupantStatus = *(int *)argument;
    return NULL;
}

// Prints the state of the houses. Used for visulaization.
void *House::printCurrentState(void *argument) {
    ostringstream convert;
    if(printOutput == true){
      convert << "Turn[" << *(int *)argument << "], cell[" << index[0] << "][" << index[1] 
              << "] = " << occupantStatus;
      MASS_base::log( convert.str());
  }

    return NULL;
}

void *House::displayStatusAsOut() {
    outMessage_size = sizeof( int );
    outMessage = new int( );
    *(int *)outMessage = (int)occupantStatus;

    return NULL;
}

void *House::receiveNeigborStatus() {
    //Only need to update neighbor values if the house is occupied
    if(agents.size() == 0)
        return NULL;
    
    for (int i=0; i< NUM_NEIGHBORS; i++) {
        int *ptr = (int *)getOutMessage( 1, neighborCoord[i] );
        neighborStatus[i] = ( ptr == NULL ) ? -1 : *ptr;
    }
    
    return NULL;
}

void *House::getSimilarityPerc() //to let residing agent know the similarity of neighbors
{
    int numOccupied = 0;
    int numSimilar = 0;

    for (int i=0; i<NUM_NEIGHBORS; i++) {
        if (neighborStatus[i] > 0) {
            numOccupied++;
            if (neighborStatus[i] == occupantStatus) {
                numSimilar++;
            }
        }  
    }
    int similarityPerc;
    if (numOccupied > 0 ) {
        similarityPerc = 100*numSimilar / numOccupied;
    }
    else {
        similarityPerc = 100;
    }
    

    int *retVal = new int();
    *(int *)retVal = similarityPerc;
    
    return retVal;
}

void *House::getHouseReservation() {  //find an available home for the agent to move
    int *retVal;
    retVal = new int[2];

    vector<int> moveset;
    int *emptyStatus = (int *)getEmptyNeighbors();

    for(int i = 0; i < NUM_EXTENDED_NEIHBORS; i++)
    {
        if(emptyStatus[i] == 0)  //cell is empty
            moveset.push_back(i);
    }

    if(!moveset.empty())
    {
        int r = rand() % moveset.size();
        retVal[0] = extendedNeighbors[moveset[r]][0];   
        retVal[1] = extendedNeighbors[moveset[r]][1];
        
    }
    else {
        int x_coord = rand() % x_size;
        int y_coord = rand() % y_size;

        retVal[0] = x_coord - index[0];   
        retVal[1] = y_coord - index[1];

    }
    return retVal;
}


void *House::getEmptyNeighbors() {
    
    int *emptyStatus = new int[NUM_EXTENDED_NEIHBORS];  //0 - empty, 1 or 2 - full

    //get the status for all coord in the vector
    for (int i=0; i< NUM_EXTENDED_NEIHBORS; i++) {

        int coord[2] = {extendedNeighbors[i][0], extendedNeighbors[i][1]};
        int *ptr = (int *)getOutMessage( 1, coord );
        emptyStatus[i] = ( ptr == NULL ) ? -1 : *ptr;
    }
    
    return emptyStatus;
}


void *House::getPrimaryResident() {
    
    if (agents.size() > 0) {
        int id = *(int*)(agents[0] -> callMethod(6, NULL));
        int *retVal = new int(id) ;
        return retVal;
    }
    
    return NULL;
}