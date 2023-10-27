#include "lib.h"



void terminate(int number){
	if(number == 1){
		printf("\nApplication terminated normally (we hope!):\n");
	}
	else{
		printf("\nApplication terminated with error code: %i\n", number);
	}

	while(1); //get stuck here
}

