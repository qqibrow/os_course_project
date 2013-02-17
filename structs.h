#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct tagPacket
{
	int requiredToken;
	int waitingTime;
	
}Packet;

typedef struct tagInput
{
	double lambda, mu, r;
	int B, P, numToArrive;
	char fileName[1024];
} Input;

typedef struct tagInputItem
{

} InputItem;

typedef struct tagMiniSeconds
{
	int decimals, integer;
}MiniSeconds;






#endif