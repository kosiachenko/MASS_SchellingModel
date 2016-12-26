#include "Resident.h"
#include "House.h"
#include "MASS_base.h"
#include <sstream>     // ostringstream
#include <math.h>      // ceil

//Toggles output for user program
const bool printOutput = true;
//const bool printOutput = true;

extern "C" Agent* instantiate( void *argument ) {
  return new Resident( argument );
}

extern "C" void destroy( Agent *object ) {
  delete object;
}

/**
 * initialize a Resident with the given argument.
 */
void *Resident::agentInit( void *argument ) {
    status = rand()%2 + 1;  //randomly select 1 or 2
    satisfied = false;
    primaryResident = false;

    int* threshold_a = (int*)argument;
    threshold = *threshold_a;

    ostringstream convert;
    if(printOutput == true){
      convert << "agentInit[" << agentId << "] created. current Location = [" << place->index[0] << "][" << place->index[1] << "]. Status ->" << status << endl;
      MASS_base::log( convert.str( ) );
    }
    return NULL;
}

void *Resident::resetForNewTurn(void *argument ) {
    if (migratableDataSize != 0) {
        int* threshold_a = (int*)argument;
        threshold = *threshold_a;

        int *params = (int*)migratableData;
        status = *params;

        migratableDataSize = 0;
        migratableData = NULL;
    }

    satisfied = false;
    primaryResident = false;

    return NULL;
}


void *Resident::communicateToHouse() {
    if (primaryResident) {
        int *statusToPass = new int;
        *statusToPass = status;

        place->callMethod(House::setOccupancy_, statusToPass);
    }
    
    
    return NULL;
}

void *Resident::calculateSatisfaction() {
    if (primaryResident) {
        int similarity = *(int*)(place->callMethod(House::getSimilarityPerc_, NULL));
        if (similarity >= threshold) {
            satisfied = true;
        }
        else {
            satisfied = false;
        }
    }
    else {
        satisfied = false; //have to move regardless
    }
    

    return NULL;
}

void *Resident::changeHome(void *argument) {
    if (!satisfied) {
        ostringstream convert;
        //save data into migratable data
        migratableDataSize = sizeof( int );
        migratableData = new int(status);

        //find an available place
        vector<int> dest;
        int x = 0, y = 0;
        int *nextMove = (int*)(place->callMethod(House::getHouseReservation_, NULL));
        if(nextMove != NULL)
        {
            x = place->index[0] + nextMove[0];
            y = place->index[1] + nextMove[1];
            dest.push_back( x );
            dest.push_back( y );
            migrate(dest);
            
            convert << "Moving the agent. Turn[" << *(int *)argument << "], Agent-" << agentId << " at [" << place->index[0];
            convert << ", " << place->index[1] << "] to move to [" << x << ", ";
            convert << y << "] a displacement of [" << nextMove[0] << ", " << nextMove[1] << "]";
        }
        else {
            convert << "Moving the agent: Agent-" << agentId << " at [" << place->index[0] << ", " << place->index[1] << "] could not move!";
        }
        if(printOutput) { 
            MASS_base::log( convert.str( ) ); 
        }
    }
    return NULL;
}

void *Resident::checkIfPrimaryRes() {
    int primaryId = *(int*)(place->callMethod(House::getPrimaryResident_, NULL));

    if (agentId == primaryId) {
        primaryResident = true;
    }
    else {
        primaryResident = false;

        ostringstream convert;
        convert << "AgentID: " << agentId << ". Primary resident ID on place: " << primaryId << endl;
        MASS_base::log( convert.str( ) );
    }
    
    

    return NULL;
}

void *Resident::provideID() {
    int *retVal = new int(agentId) ;
    return retVal;
}