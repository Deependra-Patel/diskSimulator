#include "variables.h"	
int cylinderSwitch(int diff){
	if(diff == 0)
		return 0;
	else return (cylinderSwitchK*diff + cylinderSwitchC);
}

int trackSwitch(int initial, int final){
	if(final == initial)
		return 0;
	return trackSwitchTime;
}

int sectorSwitch(int initial, int final){
	float oneRoundTime = (60*100)/rpm;
	if(final>initial){
		return (int)(final-initial)/(K*oneRoundTime);
	}
	else return (int)(K-(initial-final))/(K*oneRoundTime);
}
