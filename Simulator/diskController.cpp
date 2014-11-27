#include "diskController.h"

struct dchs{
	int d, c, h, s, parityDisk;
	dchs(int address){
		int blockNoInDisk = (address)/D;
		parityDisk = blockNoInDisk%(D+1);
		int temp = address%D;
		if(parityDisk<=temp)
			d = temp+1;
		else d = temp;
		c = blockNoInDisk/(M*K);
		h = (blockNoInDisk/K)%M;
		s = (blockNoInDisk)%K;
	}
};

diskController::diskController(){
	cout<<"Initializing diskController"<<endl;
	timer = 0;
	totalReads = 0;
	totalWrites = 0;
	disks = new disk*[D+1];
	diskReads = new int[D+1];
	diskWrites = new int[D+1];
	for(int i=0; i<D+1; i++){
		disks[i] = new disk();
		diskReads[i] = 0;
		diskWrites[i] = 0;
	}
}

void diskController::readSector(int diskAddress, char* data){
	totalReads++;
	dchs x = dchs(diskAddress);
	cout<<"----------readSector "<<diskAddress<<endl;
	cout<<"Reading From Disk: "<<x.d<<endl;	

	diskReads[x.d]++;
	disks[x.d]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
}
void diskController::writeSector(int diskAddress, char* data){
	totalReads+=2;
	totalWrites++;
	dchs x = dchs(diskAddress);
	cout<<"---------writeSector "<<diskAddress<<endl;
	cout<<"Reading From Disk: "<<x.parityDisk<<endl;

	diskReads[x.parityDisk]++;
	disks[x.parityDisk]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
	//cout<<"Reading From Disk: "<<x.d<<endl;

	diskReads[x.d]++;
	disks[x.d]->readRequest(x.c, x.h, x.s, timer, data);
	timer++;
	//takes xor and writes new data here
	//cout<<"Writing to Disk: "<<x.d<<endl;

	diskWrites[x.d]++;
	disks[x.d]->writeRequest(x.c, x.h, x.s, timer, data);
	timer++;
}
void diskController::updateAllDisks(){
	for(int i=0; i<D+1; i++){
		//cout<<endl<<"Printing data of disk"<<i<<endl;
		disks[i]->printAll();
		disks[i]->implementRequest();
		//cout<<endl<<"After update data of disk"<<i<<endl;
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
	updateAllDisks();
}
void diskController::printAll(){
	int totalSeeks = 0;
	int maxTime = 0;
	for(int i=0; i<D+1; i++){
		cout<<"Data of disk "<<i<<endl;
		cout<<"Timer: "<<disks[i]->diskTimer<<endl;
		cout<<"Total Reads: "<<diskReads[i]<<endl;
		cout<<"Total Writes: "<<diskWrites[i]<<endl;
		cout<<"Total Block Transfers: "<<diskReads[i]+diskWrites[i]<<endl;
		cout<<"Total seeks: "<<disks[i]->seeks<<endl<<endl;
		totalSeeks+= disks[i]->seeks;
		maxTime = max(maxTime, disks[i]->diskTimer);
	}
	cout<<"Priting DiskController Data-------------------\n";
	cout<<"Total Reads: "<<totalReads<<endl;
	cout<<"Total Writes: "<<totalWrites<<endl<<endl;
	cout<<"Total Block Transfers: "<<totalWrites+totalReads<<endl;
	cout<<"Total Seeks: "<<totalSeeks<<endl;
	cout<<"Max Time: "<<maxTime<<endl;
}