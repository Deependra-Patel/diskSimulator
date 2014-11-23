#include <iostream>
#include "diskController.h"
#include <stdlib.h>
#include <time.h>

int main(){
	diskController obj = diskController();	
	char* x = new char[2];
	x[0] = 'a';
	x[1] = 'b';
	srand(time(NULL));
	for(int i=0; i<100; i++){
		obj.requestHandler(rand()%100000, true, x);
		if(i%10==0) obj.updateAllDisks();
	}
}