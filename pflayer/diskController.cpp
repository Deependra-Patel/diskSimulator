#include "variables.h"


struct dchs{
	int d, c, h, s, parityDisk;
	dchs(int address){
		int blockNoInDisk = (address)/D;
		int parityDisk = blockInDisk%(D+1);
		d = blockNoInDisk;
		c = (diskAddress/(M*K))%N;
		h = (diskAddress/K)%M;
		s = (diskAddress)%K;
	}
};

class diskController{
	disk disks[];
	int timer;
	public diskController(){
		timer = 0;
		disks = new disk[D+1];
		for(int i=0; i<D+1; i++){
			disks[i] = new disk();
		}
	}

	String readSector(int diskAddress){
		dchs x = new dchs(diskAddress);
		disks[x.d].readRequest(x.c, x.h, x.s, timer);
		timer++;
	}
	String writeSector(int diskAddress, char* data){
		dchs x = new dchs(diskAddress);
		disks[x.parityDisk].readRequest(x.c, x.h, x.s, timer);
		timer++;
		disks[x.d].readRequest(x.c, x.h, x.s, timer);
		timer++;
		//takes xor and writes new data here
		disks[x.d].writeRequest(x.c, x.h, x.s, timer, data);
		timer++;
	}
	void updateAllDisks(){
		for(int i=0; i<D+1; i++){
			disks[i].implementRequest();
		}
	}

	void requestHandler(int diskAddress, bool requestType, char* data){
		if(requestType){ //read request
			readSector(diskAddress);
		}
		else{
			writeSector(diskAddress, data);
		}
	}
};
int main(){
	cout<<"df";
}