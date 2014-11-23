cacheController::cacheController(){
	curSize = 0;
	free = new bool[cacheSize];
	dirty = new bool[cacheSize];
	dC = new diskController();
	for(int i=0; i<cacheSize; i++){
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
	if(!list.empty()){
		int freed = list.front();
		list.pop_front();
		dC->requestHandler(diskAddress, false, "data"); //writing back dirty blocks
		dirty[freed] = false;
		return freed;
	}
	else return 0;
}
void cacheController::putPageInCache(int diskAddress, bool read){
	int freePage = getFreePage();
	diskAddressCacheMap[diskAddress] = freePage;//putting page in cache
	accessOrder.push_back(freePage);
	curSize ++;	
	if(!read)
		dirty[freePage] = true;
}
void cacheController::fetchPageFromDisk(int diskAddress){
	std::unordered_map<int, int>::const_iterator got = diskAddressCacheMap.find(diskAddress);
	if(got == diskAddressCacheMap.end()){
		cout<<"Page not found in cache"<<endl;
		dC->requestHandler(diskAddress, true, "data");
		putPageInCache(diskAddress);
	}
	else { //found
		cout<<"Page found in cache";
	}
}
void cacheController::writePageToDisk(int diskAddress){
	std::unordered_map<int, int>::const_iterator got = diskAddressCacheMap.find(diskAddress);
	if(got == diskAddressCacheMap.end()){
		cout<<"Page not found in cache"<<endl;
		putPageInCache(diskAddress);
	}
	else { //found
		cout<<"Page found in cache";
		dirty[*got] = true;
	}
}
