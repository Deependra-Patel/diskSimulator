#include "cacheController.h"

#include <map>
using namespace std;
struct mainControllerStruct
{
	map<int,int> addressmap;
	int pagesInDisk;
	cacheController cacheControllerStruct;
	mainControllerStruct();
	void diskRequest(int fileNum, int pagenum, int requestType);
	void writeBackCache();
	void printAll();
};