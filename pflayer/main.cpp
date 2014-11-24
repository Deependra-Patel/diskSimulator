#include <iostream>
#include "mainController.h"
//#include "cacheController.h"
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

	//second
	// srand(time(NULL));
	// mainControllerStruct obj = mainControllerStruct();
	// for(int i=0; i<10000; i++){
	// 	// int pageNum = rand()%(D);
	// 	for(int j=0; j<D; j++){
	// 		obj.diskRequest(i, j, 2); //write
	// 	}
	// }
	// for (int i = 0; i < 10000; ++i){
	// 	int pageNum = rand()%(D);

	// 	int ran = rand()%2;
	// 	int j = rand()%10000;
	// 	if(ran)
	// 		obj.diskRequest(j, pageNum, 2); //write
	// 	else obj.diskRequest(j, pageNum, 1); //read
	// }
	// if(cacheEffect)
	// 	//obj.writeBackCache();
	// obj.printAll();

	// cacheController *ch = new cacheController();
	// ch->writePageToDisk(2);
	// ch->writePageToDisk(3);
	// ch->writePageToDisk(2);
	// ch->writePageToDisk(4);
	// ch->writePageToDisk(5);
	// ch->writePageToDisk(1);
	// ch->printAll();

	srand(time(NULL));
	mainControllerStruct obj = mainControllerStruct();
	for(int i=0; i<tests; i++){
		for(int j=0; j<D; j++){
			obj.diskRequest(i, j, 2); //write
		}
	}
	for(int z=0; z<tests; z++){
		int i, j, k;
		cin>>i>>j>>k;
		obj.diskRequest(i, j, k+1); //write
	}
	obj.printAll();
	
}