#include <iostream>
#include "cacheController.h"
#include <stdlib.h>
#include <time.h>

int main(){
	cacheController *ch = new cacheController();
	ch->writePageToDisk(2);
	ch->writePageToDisk(3);
	ch->writePageToDisk(2);
	ch->fetchPageFromDisk(32);
	ch->fetchPageFromDisk(31);

	cout<<"Cache hit ration: "<<(float)ch->cacheHit/(float)(ch->cacheHit + ch->cacheMiss)<<endl;
}