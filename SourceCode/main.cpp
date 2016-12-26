#include "MASS.h"
#include "House.h"
#include "Resident.h"
#include "Timer.h"	//Timer
#include <stdlib.h> // atoi
#include <unistd.h>
#include <vector>

const bool printOutput = true;

Timer timer;

int main( int argc, char *args[] ) {

	// check that all arguments are present 
	// exit with an error if they are not
	if ( argc != 11 ) {
		cerr << "usage: ./main username password machinefile port nProc nThreads numTurns sizeX sizeY threshold" << endl;
		return -1;
	}
  
	// get the arguments passed in
	char *arguments[4];
	arguments[0] = args[1]; // username
	arguments[1] = args[2]; // password
	arguments[2] = args[3]; // machinefile
	arguments[3] = args[4]; // port
	int nProc = atoi( args[5] ); // number of processes
	int nThr = atoi( args[6] );  // number of threads
  
	const int numTurns = atoi( args[7] );	//Run this simulation for numTurns
	const int sizeX = atoi( args[8] );
	const int sizeY = atoi( args[9] );
	const int threshold = atoi(args[10]); //% of neighbors alike to be satisfied with place
	const double fillRate = 0.8;
	const int myPopulation = sizeX * sizeY * fillRate;	//Population
 	
	// initialize MASS with the machine information,
	// number of processes, and number of threads
	MASS::init( arguments, nProc, nThr ); 
	srand (time(NULL));  //to make random sequence different for each process and thread

	// prepare a message for the places (this is argument below)
	char *msg = (char *)("hello\0"); // pass argument to constructors
  
	// Create the places. Arguments are, in order:
	//    handle, className, boundary_width, argument, argument_size, dim, ...
	Places *houses = new Places(1, "House", 3, msg, 6, 2, sizeX, sizeY );

	// Create the agents. Arguments are, in order:
	//    handle, className, *argument, argument_size, *places,  initPopulation
	Agents *residents = new Agents(2, "Resident", msg, 6, houses, myPopulation );
  
  	std::cerr << "Agents and Places created " << std::endl;

  	int* threshold_p = new int;
	*threshold_p = threshold;

	houses->callAll(House::init_);				//initialize Houses
	residents->callAll(Resident::init_, (void*) threshold_p, sizeof(int) );        //initialize Residents

	std::cerr << "Agents and Places initialized " << std::endl;
  
	timer.start();
		
	for(int turn = 0; turn < numTurns; turn++)
	{
		int* curTurn = new int;
		*curTurn = turn;

		houses->callAll(House::resetForNewTurn_);
		residents->callAll(Resident::resetForNewTurn_, (void*) threshold_p, sizeof(int) );

		residents->callAll( Resident::checkIfPrimaryRes_);	//Check if there are any other agents residing on the same place

		residents->callAll( Resident::communicateToHouse_);	//Communicate their economic status
		houses->callAll( House::printCurrentState_, (void*) curTurn, sizeof(int) );

		houses->callAll(House::displayStatusAsOut_);	// display the current status
		houses->exchangeBoundary();		// exchange shadow space information
		houses->callAll(House::receiveNeigborStatus_);  //receive statuses of houses in the neihborhood

		residents->callAll( Resident::calculateSatisfaction_);

		//Migrate unsatiffied residents:
		residents->callAll( Resident::changeHome_, (void*) curTurn, sizeof(int) );
		residents->manageAll();  //complete migration
		
		
		std::cerr << "Finished turn " << turn << std::endl;
	}
	
	long elaspedTime_END =  timer.lap();
	printf( "\nEnd of simulation. Elasped time using MASS framework with %i processes and %i thread and %i turns :: %ld \n",nProc,nThr,numTurns, elaspedTime_END);
  
	MASS::finish( );
}
