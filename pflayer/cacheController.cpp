#include "cacheController.h"
cacheController::cacheController(){
	curSize = 0;
	free = new bool[cacheSize];
	dirty = new bool[cacheSize];
	diskAddressOfPage = new int[cacheSize];
	dC = new diskController();
	cacheHit = 0;
	cacheMiss = 0;
	for(int i=0; i<cacheSize; i++){
		diskAddressOfPage[i] = -1;
		free[i] = true;
		dirty[i] = false;
	}
}
int cacheController::getFreePage(){
	for(int i=0; i<cacheSize; i++){ //if any page is free
		if(free[i]){
			return i;
		}
	}
	curSize --;
	for(int i=0; i<cacheSize; i++){ //if page is not dirty
		if(!dirty[i]){
			return i;
		}
	}
	 //Have to remove any dirty page using lru
	if(!accessOrder.empty()){
		int freed = accessOrder.front();
		accessOrder.pop_front();
		int diskAddress = diskAddressOfPage[freed];
		char data[100];
		strcpy(data, "data");
		dC->requestHandler(diskAddress, false, data); //writing back dirty blocks
		dirty[freed] = false;
		return freed;
	}
	else return 0;
}
void cacheController::putPageInCache(int diskAddress, bool read){
	cout<<"putPageInCache \n";
	int freePage = getFreePage();
	cout<<"Putting in "<<freePage<<endl;
	free[freePage] = false;
	diskAddressCacheMap[diskAddress] = freePage;//putting page in cache
	accessOrder.push_back(freePage);
	curSize ++;	
	diskAddressOfPage[freePage] = diskAddress;
	if(!read)
		dirty[freePage] = true;
}
void cacheController::fetchPageFromDisk(int diskAddress){
	cout<<"fetchPageFromDisk: "<<diskAddress<<endl;
	char data[100];
	strcpy(data, "data");
	if(cacheEffect){
		std::unordered_map<int, int>::const_iterator got = diskAddressCacheMap.find(diskAddress);
		if(got == diskAddressCacheMap.end()){
			cacheMiss++;
			cout<<"Page not found in cache"<<endl;
			char data[100];
			strcpy(data, "data");
			dC->requestHandler(diskAddress, true, data);
			putPageInCache(diskAddress, true);
		}
		else { 
			cacheHit++;
			cout<<"Page found in cache"<<endl;
		}
	}
	else {
		cacheMiss++;
		dC->requestHandler(diskAddress, true, data); //directly reading	
	}
}
void cacheController::writePageToDisk(int diskAddress){
	char data[100];
	strcpy(data, "data");
	cout<<"writePageToDisk: "<<diskAddress<<endl;
	if(cacheEffect){
		std::unordered_map<int, int>::const_iterator got = diskAddressCacheMap.find(diskAddress);
		if(got == diskAddressCacheMap.end()){
			cacheMiss++;
			cout<<"Page not found in cache"<<endl;
			putPageInCache(diskAddress, false);
		}
		else { //found
			cacheHit++;
			cout<<"Page found in cache"<<endl;
			dirty[got->second] = true;
		}
	}
	else {
		cacheMiss++;
		dC->requestHandler(diskAddress, false, data); //directly writing
	}
}
void cacheController::writeBackAllDirty(){
	cout<<"Writing cache:::"<<endl;
	for(int i=0; i<cacheSize; i++){
		if(dirty[i]){
			int diskAddress = diskAddressOfPage[i];
			char data[100];
			strcpy(data, "data");
			cout<<diskAddress<<endl;
			dC->requestHandler(diskAddress, false, data);
		}
	}
}
void cacheController::reset(){
	cacheHit =0;
	cacheMiss =0;
}
void cacheController::printAll(){
	cout<<"Priting Cache Data-------------------\n";
	cout<<"Cache Hits: "<<cacheHit<<endl;
	cout<<"Cache Miss: "<<cacheMiss<<endl;
	cout<<"Cache hit Ratio: "<<(float)(cacheHit)/(float)(cacheHit + cacheMiss)<<endl;
	dC->printAll();
}
