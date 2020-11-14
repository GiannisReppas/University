
#define TIME_PORT_FUNCTIONS 30

#define NUMBER_OF_VESSELS 3

#define TIME 5
#define STATTIMES 15

typedef struct vessel
{
	int vesselId;
	time_t startWaitTime;
	time_t arriveTime;
	time_t leaveTime;
	char placeType[2];
	char shipType[2];
	bool status;
	int cost;
}Vessel;
