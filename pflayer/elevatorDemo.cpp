#include "diskRequestHandler.h"
#include <iostream>
using namespace std;

int main(){
	disk *obj = new disk();
	char data[10];
	obj->readRequest(1, 2, 3, 0, data);
	obj->printAll();
	obj->readRequest(11, 14, 4, 1, data);
	obj->printAll();
	obj->readRequest(5, 43, 31, 2, data);
	obj->printAll();

	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();
	obj->readRequest(12, 21, 3, 4, data);
	obj->printAll();
	obj->readRequest(3, 21, 3, 4, data);

	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();	
	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();	
	obj->readRequest(56, 21, 3, 4, data);
	obj->printAll();
	obj->readRequest(91, 21, 3, 4, data);
	obj->printAll();
	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();	
	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();
	cout<<"Execute one step \n";
	obj->implementRequest();
	obj->printAll();

}