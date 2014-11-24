#include "diskController.h"
#include <string.h>
#include <unordered_map>
#include <list>
struct  cacheController{
	bool *dirty;
	bool *free;
	list<int> accessOrder;
	unordered_map<int, int> diskAddressCacheMap;
	int curSize;
	int cacheHit;
	int cacheMiss;
	diskController* dC;
	int *diskAddressOfPage;
	cacheController();
	int getFreePage();
	void putPageInCache(int diskAddress, bool read);
	void fetchPageFromDisk(int diskAddress);
	void writePageToDisk(int diskAddress);
	void writeBackAllDirty();
	void printAll();
	void reset();
};