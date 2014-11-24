#include <iostream>
#include "mainController.h"
#include <stdlib.h>
#include <time.h>

int main(){
	// diskController obj = diskController();	
	// char* x = new char[2];
	// x[0] = 'a';
	// x[1] = 'b';
	// srand(time(NULL));
	// for(int i=0; i<100; i++){
	// 	obj.requestHandler(rand()%100000, true, x);
	// 	if(i%10==0) obj.updateAllDisks();
	// }
	srand(time(NULL));
	mainControllerStruct obj = mainControllerStruct();
	for (int i = 0; i < 10000; ++i){
		int pageNum = rand()%(D);

		int ran = rand()%2;
		if(ran)
			obj.diskRequest(i, pageNum, 2); //write
		else obj.diskRequest(i, pageNum, 1); //read
	}
	obj.writeBackCache();
}