#include <iostream>
#include "mainController.h"
//#include "cacheController.h"
#include <stdlib.h>
#include <time.h>

int main(){
	srand(time(NULL));
	mainControllerStruct obj = mainControllerStruct();
	for(int i=0; i<tests; i++){
		for(int j=0; j<D; j++){
			obj.diskRequest(i, j, 2); //write
		}
	}
	cout<<"Total Cache hits, misses after initializing: "<<obj.cacheControllerStruct.cacheHit<<", "<<obj.cacheControllerStruct.cacheMiss<<endl;
	for(int z=0; z<tests; z++){
		int i, j, k;
		cin>>i>>j>>k;
		obj.diskRequest(i, j, k+1); //write
	}
	int totCacheHits = obj.cacheControllerStruct.cacheHit;
	int totCacheMiss = obj.cacheControllerStruct.cacheMiss;
	cout<<"Total Cache hits, misses after running: "<<totCacheHits<<", "<<totCacheMiss<<endl;	
	cout<<"Actual Cache hit rate after running: "<<(float)(totCacheHits)/(float)(totCacheHits + totCacheMiss - tests*D)<<endl;
	obj.printAll();
}