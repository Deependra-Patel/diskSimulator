#include "variables.h"
#include "diskRequestHandler.h"
struct diskController{
	disk** disks;
	int timer;
	int *diskReads;
	int *diskWrites;
	diskController();
	void readSector(int diskAddress, char* data);
	void writeSector(int diskAddress, char* data);
	int totalReads;
	int totalWrites;
	void updateAllDisks();
	void requestHandler(int diskAddress, bool requestType, char* data);
	void printAll();

};