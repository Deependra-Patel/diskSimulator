#include "variables.h"
#include "diskRequestHandler.h"
struct diskController{
	disk** disks;
	int timer;
	diskController();
	void readSector(int diskAddress, char* data);
	void writeSector(int diskAddress, char* data);
	void updateAllDisks();
	void requestHandler(int diskAddress, bool requestType, char* data);
};