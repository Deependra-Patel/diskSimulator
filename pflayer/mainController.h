#include "cacheController.h"

#include <map>
#include <stdlib.h>
#include <time.h>

using namespace std;
struct mainControllerStruct
{
	map<int,int> addressmap;
	int pagesInDisk;
	cacheController* cacheControllerStruct;
	void diskRequest(int fileNum, int pagenum, bool requestType);
}