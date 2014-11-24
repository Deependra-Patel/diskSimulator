#include "diskRequestHandler.h"
#include "functions.cpp"


CHSRequest::CHSRequest(int c, int h, int s, int t, bool type, char* d){
	cylinderNum = c;
	headNum = h;
	sectorNum = s;
	rTime = t;
	data = d;
	requestType = type;
}

bool CHSRequest::operator<(const CHSRequest &b) const
{
    if (cylinderNum == b.cylinderNum)
        return (rTime < b.rTime);
    return (cylinderNum < b.cylinderNum);
}
disk::disk(){
	cout<<"Initializing disk"<<endl;
	traversalDirection = true; // true means outwards, 0 -> most inward
	itr = cylinderRequestsSet.begin();
	diskTimer = 0;
	previousCylinder = 0;
	previousTrack = 0;
	previousSector = 0;
	seeks = 0;
}
// elevatorEffect

void disk::putRequest(int cNum, int tNum, int sNum, int t, bool requestType, char* data){
	if (elevatorEffect){
		//cout<<"Putting request(with elevator) (cNum, tNum, sNum, t, requestType, data) "<<cNum<<", "<<tNum<<", "<<sNum<<", "<<t<<", "<<requestType<<", "<<data<<", "<<endl;
		cylinderRequestsSet.insert(CHSRequest(cNum, tNum, sNum, t, requestType, data));
	}
	else{
		//cout<<"Putting request(without elevator) (cNum, tNum, sNum, t, requestType, data) "<<cNum<<", "<<tNum<<", "<<sNum<<", "<<t<<", "<<requestType<<", "<<data<<", "<<endl;
		withoutElevatorList.push_back(CHSRequest(cNum, tNum, sNum, t, requestType, data));
	}
}

void disk::readRequest(int cNum, int tNum, int sNum, int t, char* data){
	putRequest(cNum, tNum, sNum, t, true, data);
}


void disk::writeRequest(int cNum, int tNum, int sNum, int t, char* data){
	putRequest(cNum, tNum, sNum, t, false, data);
}

void disk::implementRequest(){
	CHSRequest currentRequest;
	if (elevatorEffect){
		if (cylinderRequestsSet.empty()) return;
		if (itr == cylinderRequestsSet.end()){
				itr--;
				traversalDirection = false;
			}
		if (traversalDirection){
			
			currentRequest = *itr;
			set<CHSRequest>::iterator temp = itr;
			temp++;
			cylinderRequestsSet.erase(itr);
			itr = temp;
		}
		else{

			currentRequest = *itr;
			if (itr == cylinderRequestsSet.begin()){
				set<CHSRequest>::iterator temp = itr;
				temp++;
				cylinderRequestsSet.erase(itr);
				itr = temp;
				traversalDirection = true;
			}
			else{
				set<CHSRequest>::iterator temp = itr;
				temp--;
				cylinderRequestsSet.erase(itr);
				itr = temp;
			}
		}
		if(currentRequest.cylinderNum != previousCylinder)
			seeks++;
	}
	else{
		if (withoutElevatorList.empty()) return;
		currentRequest = withoutElevatorList.front();
		withoutElevatorList.pop_front();
		seeks++;
	}

	diskTimer += cylinderSwitch(abs(currentRequest.cylinderNum - previousCylinder)) + 
				trackSwitch(currentRequest.headNum, previousTrack) + sectorSwitch(currentRequest.sectorNum, previousSector);

	previousSector = currentRequest.sectorNum;
	previousTrack = currentRequest.headNum;
	previousCylinder = currentRequest.cylinderNum;
}

void disk::printAll(){
	if (elevatorEffect){
		set<CHSRequest>::iterator it = cylinderRequestsSet.begin();
		while(it!=cylinderRequestsSet.end()){
			cout << it->cylinderNum << ", ";
			it++;
		}
	}
	else{
		list <CHSRequest>::iterator it = withoutElevatorList.begin();
		while(it!=withoutElevatorList.end()){
			cout << it->cylinderNum << ", ";
			it++;
		}	
	}
	cout<<endl;
}