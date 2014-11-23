#include "diskController.h"

struct dchs{
	int d, c, h, s, parityDisk;
	dchs(int address){
		int blockNoInDisk = (address)/D;
		int parityDisk = blockNoInDisk%(D+1);
		int temp = address%D;
		if(parityDisk<=temp)
			d = temp+1;
		else d = temp;
		c = (blockNoInDisk/(M*K))%N;
		h = (blockNoInDisk/K)%M;
		s = (blockNoInDisk)%K;
	}
};

diskController::diskController(){
	cout<<"Initializing disk diskController"<<endl;
	timer = 0;
	disks = new disk*[D+1];
	for(int i=0; i<D+1; i++){
		disks[i] = new disk();
	}
}

void diskController::readSector(int diskAddress, char* data){
	dchs x = dchs(diskAddress);
	cout<<"----------readSector "<<diskAddress<<endl;
	cout<<"Reading From Disk: "<<x.d<<endl;	
	disks[x.d]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
}
void diskController::writeSector(int diskAddress, char* data){
	dchs x = dchs(diskAddress);
	cout<<"---------writeSector"<<diskAddress<<endl;
	cout<<"Reading From Disk: "<<x.parityDisk<<endl;
	disks[x.parityDisk]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
	cout<<"Reading From Disk: "<<x.d<<endl;
	disks[x.d]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
	//takes xor and writes new data here
	cout<<"Writing to Disk: "<<x.d<<endl;
	disks[x.d]->writeRequest(x.c, x.h, x.s, timer, data);
	timer++;
}
void diskController::updateAllDisks(){
	for(int i=0; i<D+1; i++){
		cout<<endl<<"Printing data of disk"<<i<<endl;
		disks[i]->printAll();
		disks[i]->implementRequest();
		cout<<endl<<"After update data of disk"<<i<<endl;
		disks[i]->printAll();
	}
}

void diskController::requestHandler(int diskAddress, bool requestType, char* data){
	if(requestType){ //read request
		readSector(diskAddress, data);
	}
	else{
		writeSector(diskAddress, data);
	}
}