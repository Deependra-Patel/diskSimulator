#include <stdio.h>
#include <iostream>
#include <set>
#include <list>
#include <cmath>
#include "variables.h"



struct CHSRequest{
	int cylinderNum, headNum, sectorNum, rTime;
	bool requestType;
	char* data;
	CHSRequest(int c, int h, int s, int t, bool type, char* d);
	CHSRequest(){}
	bool operator < (const CHSRequest &b) const;
};

struct disk
{
	int diskTimer;
	int seeks;
	set<CHSRequest> cylinderRequestsSet;
	list<CHSRequest> withoutElevatorList;
	bool traversalDirection; 
	set<CHSRequest>::iterator itr;

	int previousCylinder, previousTrack, previousSector;
	disk();
	void putRequest(int cNum, int tNum, int sNum, int t, bool requestType, char* data);
	void readRequest(int cNum, int tNum, int sNum, int t, char* data);
	void writeRequest(int cNum, int tNum, int sNum, int t, char* data);
	void implementRequest();
	void printAll();
};
