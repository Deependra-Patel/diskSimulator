#include "diskController.h"
#include <string.h>
#include <unordered_map>
#include <list>
struct  cacheController{
	bool *dirty; //array to store if page is dirty
	bool *free; //to store if page is free
	list<int> accessOrder; //list to store sequence of requests
	unordered_map<int, int> diskAddressCacheMap; //mapping of diskAddress to cache block
	int curSize; 
	int cacheHit;
	int cacheMiss;
	diskController* dC;
	int *diskAddressOfPage; //stores diskAddress of a given page in cache
	cacheController();
	int getFreePage();
	void putPageInCache(int diskAddress, bool read);
	void fetchPageFromDisk(int diskAddress);
	void writePageToDisk(int diskAddress);
	void writeBackAllDirty();
	void printAll();
	void reset();
};