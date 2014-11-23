// #include "diskRequestHandler.h"

#include <stdio.h>
#include <set>
#include <math.h>

struct CHSRequest
{
	int cylinderNum, headNum, sectorNum, rTime;
	CHSRequest(int c, int h, int s, int t){
		cylinderNum = c;
		headNum = h;
		sectorNum = s;
		rTime = t;
	}
};

struct requestComparer
{
    bool operator()(const CHS &a, const CHS &b)
    {
        if (a.cylinderNum == b.cylinderNum)
            return (a.rTime < b.rTime);
        return (a.cylinderNum < b.cylinderNum);
    }
};

struct disk
{
	int diskNumber;
	int diskTimer;
	set<int, requestComparer> cylinderRequestsSet;
	bool traversalDirection; 
	set<int, requestComparer>::iterator itr;

	int previousCylinder, previousTrack, previousSector;
	disk(int dNum){
		diskNumber = dNum;
		traversalDirection = true; // true means outwards, 0 -> most inward
		itr = cylinderRequestsSet.begin();
		diskTimer = 0;
		previousCylinder = 0;
		previousTrack = 0;
		previousSector = 0;
	}

	void putRequest(int cNum, int tNum, int sNum, int t){
		cylinderRequestsSet.insert(new CHSRequest(cNum, tNum, sNum, t));
	}

	void implementRequest(){
		if (cylinderRequestsSet.empty()) return;
		if (itr == cylinderRequestsSet.end()){
				itr--;
				traversalDirection = false;
			}
		CHSRequest currentRequest;
		if (traversalDirection){
			
			currentRequest = *itr;
			set<int, requestComparer>::iterator temp = itr;
			temp++;
			cylinderRequestsSet.erase(itr);
			itr = temp;
		}
		else{

			currentRequest = *itr;
			if (itr == cylinderRequestsSet.begin()){
				set<int, requestComparer>::iterator temp = itr;
				temp++;
				cylinderRequestsSet.erase(itr);
				itr = temp;
				traversalDirection = true;
			}
			else{
				set<int, requestComparer>::iterator temp = itr;
				temp--;
				cylinderRequestsSet.erase(itr);
				itr = temp;
			}
		}
		diskTimer += cylinderSwitch(abs(currentRequest.cylinderNum - previousCylinder)) + 
					trackSwitch(currentRequest.headNum, previousTrack) + sectorSwitch(currentRequest.sectorNum, previousSector);

		previousSector = currentRequest.sectorNum;
		previousTrack = currentRequest.headNum;
		previousCylinder = currentRequest.cylinderNum;
	}
};
