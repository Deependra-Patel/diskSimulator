#include "mainController.h"

mainControllerStruct::mainControllerStruct(){
	pagesInDisk = D*M*K;
}

// fileNum, pagenum -> diskaddress
void mainControllerStruct::diskRequest(int fileNum, int pagenum, int requestType){
	srand(time(NULL));
	map<int,int>::iterator it = addressmap.find(pagenum);

	if (requestType == 1){
		/**
			Read Request should be already inserted in the file
		*/

		
		if (it == addressmap.end()){
			printf("%s\n", "No such file exists in disk. Bad access Request.");
		}
		else{
			fetchPageFromDisk((fileNum * pagesInDisk) + it->second);	
		}
		
	}
	else if(requestType == 2){

		/**
			function to recover a obtain mapping from fillenumber, pagenumber 
		*/

		int answer;

		if (it != addressmap.end()){
			answer = (fileNum*pagesInDisk) + it->second;
		}
		else{
			int newPageNum = rand()%pagesInDisk;
			map[pagenum] = newPageNum;	// Insert entry in map
			answer = (fileNum*pagesInDisk) + newPageNum;
		}

		writePageToDisk(answer);
	}
	else{
		/**
			If a page is deleted then that disk space is emptied. The mapping is removed as it is no longer 
			needed for finding the disk address
		*/
		if (it != addressmap.end()){
			addressmap.erase(it);
		}
	}

}
