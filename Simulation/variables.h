#define D 5 //no of disks Also change in testData.py
#define N 1000 //no. of cylinders
#define M 10 //no. of tracks/cylinder
#define K 100 //no. of sectors/track

#define cylinderSwitchK 0.01 //Variables to calculate cylinder switch time
#define cylinderSwitchC 2 //switch time = K*diff + C. Please refer functions.cpp

#define trackSwitchTime 0 //Variables for track switch time

#define rpm 10000 //Rounds per minute for hard disk
 
#define cacheSize 100

#define cacheEffect true

#define elevatorEffect true

#define tests 1000 //Also change in testData.py

using namespace std;