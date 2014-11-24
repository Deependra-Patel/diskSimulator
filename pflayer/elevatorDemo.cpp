#include "diskRequestHandler.h"

#include <iostream>
using namespace std;
void print(disk* obj){
	set<CHSRequest>::iterator it = obj->cylinderRequestsSet.begin();
	while(it!=obj->cylinderRequestsSet.end()){
		cout << it->cylinderNum << ", ";
		it++;
	}
	cout<<endl;
}
int main(){
	disk *obj = new disk();
	char data[10];
	obj->readRequest(1, 2, 3, 0, data);
	print(obj);
	cout<<"Inserted 1"<<endl;
	obj->readRequest(11, 14, 4, 1, data);
	print(obj);
	cout<<"Inserted 11"<<endl;
	obj->readRequest(5, 43, 31, 2, data);
	print(obj);
	cout<<"Inserted 5"<<endl;

	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);
	obj->readRequest(12, 21, 3, 4, data);
	print(obj);
	cout<<"Inserted 12"<<endl;
	obj->readRequest(3, 21, 3, 4, data);
	print(obj);
	cout<<"Inserted 3"<<endl;

	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);	
	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);	
	obj->readRequest(56, 21, 3, 4, data);
	print(obj);
	cout<<"Inserted 56"<<endl;
	obj->readRequest(91, 21, 3, 4, data);
	print(obj);
	cout<<"Inserted 91"<<endl;
	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);	
	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);
	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);
	cout<<"Execute one step \n";
	obj->implementRequest();
	print(obj);
	cout<<"Total Time taken: "<<obj->diskTimer<<endl;

}