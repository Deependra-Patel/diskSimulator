#include "diskController.h"
#include <unordered_map>
#include <list>
struct  cacheController{
	bool *dirty;
	bool *free;
	list<int> accessOrder;
	unordered_map<int, int> diskAddressCacheMap;
	int curSize;
	diskController* dC = new diskController();
	cacheController();
	int getFreePage();
	void putPageInCache(int diskAddress, bool read);
	void fetchPageFromDisk(int diskAddress);
	void writePageToDisk(int diskAddress);
}