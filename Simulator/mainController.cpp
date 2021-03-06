#include "mainController.h"

mainControllerStruct::mainControllerStruct(){
	pagesInDisk = N*M*K;
}

// fileNum, pagenum -> diskaddress
void mainControllerStruct::diskRequest(int fileNum, int pagenum, int requestType){
	map<int,int>::iterator it = addressmap.find(fileNum);
	set<int> takenAddresses;

	if (requestType == 1){
		/**
			Read Request should be already inserted in the file
		*/

		
		if (it == addressmap.end()){
			printf("%s\n", "No such file exists in disk. Bad access Request.");
		}
		else{
			cacheControllerStruct.fetchPageFromDisk((pagenum * pagesInDisk) + it->second);	
		}

	}

	else if(requestType == 2){

		/**
			function to recover a obtain mapping from fillenumber, pagenumber 
		*/

		int answer;

		if (it != addressmap.end()){
			answer = (pagenum*pagesInDisk) + it->second;
		}
		else{
			
			int newFileNum = rand()%pagesInDisk;
			
			while (takenAddresses.find(newFileNum) != takenAddresses.end()){
				newFileNum = rand()%pagesInDisk;
			}

			takenAddresses.insert(newFileNum);

			addressmap[fileNum] = newFileNum;	// Insert entry in map
			answer = (pagenum*pagesInDisk) + newFileNum;
		}

		cacheControllerStruct.writePageToDisk(answer);
	}
	else{
		
		/**
			If a page is deleted then that disk space is emptied. The mapping is removed as it is no longer 
			needed for finding the disk address
		*/

		if (it != addressmap.end()){
			addressmap.erase(it);
			takenAddresses.erase(it->second);
		}
	}

}

void mainControllerStruct::writeBackCache(){
	cacheControllerStruct.writeBackAllDirty();
}

void mainControllerStruct::printAll(){
	cacheControllerStruct.printAll();
}
