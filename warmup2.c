#include "structs.h"
#include "queue.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

typedef struct timeval TimeVal;


TimeVal emulationStartTime, curTime;
struct timezone tz;



// double getCurrentMicroSeconds()
// {

// 	gettimeofday(&tv, &tz);
// 	return tv.tv_sec*1000 + tv.tv_usec/1000;
// }

Packet* createPacket()
{
	Packet* p = (Packet*)malloc(sizeof(Packet));
	return p;
}

useconds_t turnRateToMicroSeconds(double rate)
{
	return 1000000 * ( 1.0 / rate);
}



//global variable including mutex
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int deathFlag = FALSE;
pthread_cond_t newPacketToServer = PTHREAD_COND_INITIALIZER;
FILE* fptr = NULL;

Input input;
MyQueue arrivalQueue, serverQueue;
unsigned int g_tokens = 0;
unsigned int g_tokenSize;
pthread_t arrive_thread, server_thread, token_thread;



int parseParams(int argc, char* argv[], Input* input)
{
	 for (int i = 1; i < argc; ++i) { 
        if (i + 1 == argc)
        	break;

        if (TRUE == strncmp(argv[i], "-lambda", 7)) 
        {	
            input->lambda = atof(argv[i + 1]);  //lambda
        } 
        else if (TRUE == strncmp(argv[i], "-mu", 3)) 
        {
            input->mu = atof(argv[i+1]);  		//mu
        } 
        else if (TRUE == strncmp(argv[i], "-r", 2)) 
        {
            input->r = atof(argv[i+1]); 		// r
        } 
        else if (TRUE == strncmp(argv[i], "-B", 2)) 
        {
            input->B= atoi(argv[i+1]); 			// B
        } 
        else if (TRUE == strncmp(argv[i], "-P", 2)) 
        {
            input->P = atoi(argv[i+1]); 		// P
        } 
        else if (TRUE == strncmp(argv[i], "-n", 2)) 
        {
            input->numToArrive = atoi(argv[i+1]); 	// n
        } 
        else if (TRUE == strncmp(argv[i], "-t", 2)) 
        {
        	strcpy(input->fileName, argv[i+1]);
        } 
    }

    return TRUE;
}


int isNonNegitive(int num)
{
	static int MAX = 0x7fffffff;
	return num > 0 && num < MAX;
}

int checkInput(Input* input)
{
	if(input->lambda < 0 || input->mu < 0 || input->r < 0)
	{
		fprintf(stderr, "some double parameters is less than 0\n");
		return FALSE;
	}
	if( !isNonNegitive(input->B) || !isNonNegitive(input->P) ||
		!isNonNegitive(input->numToArrive))
	{
		fprintf(stderr, "some int parameters is too large or less than 0\n" );
		return FALSE;
	}

	if( NULL == (fptr = fopen(input->fileName, "r")) )
	{
		fprintf(stderr, "tfile cannot be open\n" );
		return FALSE;
	}


	return TRUE;
}

useconds_t diff2Timeval(TimeVal start, TimeVal end)
{
	return (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
}

 void printMiniSeconds(TimeVal start, TimeVal end)
 {
 	int integer_minisecond, decimals_minisecond;
 	useconds_t diff = diff2Timeval(start, end);
 	integer_minisecond = diff / 1000;
 	decimals_minisecond = diff % 1000;

 	printf("%08d.%03dms: ", integer_minisecond, decimals_minisecond);
 }

void* arriveThread(void* arg)
{
	useconds_t sleepTime; //time to sleep
	TimeVal startTime, endTime;
	
	int emptyFlag;
	int waitingTime;

	sleepTime = -1;
	emptyFlag = -1;
	waitingTime = -1;

	while(TRUE)
	{
		if(-1 == usleep(sleepTime))
		{
			printf("something wrong with usleep\n");
		}

		gettimeofday(&startTime, &tz);

		Packet* p = createPacket();

		if(p->requiredToken > g_tokenSize)
			continue;


		// ******start critical section*************
		pthread_mutex_lock(&m);
		emptyFlag = MyQueueEmpty(&arrivalQueue);
		MyQueueEnqueue(&arrivalQueue, (void*)p);

		if (emptyFlag && p->requiredToken <= g_tokens)
		{
			g_tokens -= p->requiredToken;
			MyQueueDequeue(&arrivalQueue);
			MyQueueEnqueue(&serverQueue, (void*)p);

			if( 1 == My402ListLength(&serverQueue.list))
			{
				pthread_cond_signal(&newPacketToServer);
			}

		}
		pthread_mutex_unlock(&m);
		// *********end critical section*************
		gettimeofday(&endTime, &tz);


		// read file here if needed
		sleepTime = waitingTime - diff2Timeval(startTime, endTime);


	}
}

void* serverThread(void* arg)
{
	useconds_t serviceTime;
	Packet* p = NULL;
	serviceTime = turnRateToMicroSeconds(input.mu);
	while(TRUE)
	{
		pthread_mutex_lock(&m);
		pthread_cond_wait(&newPacketToServer, &m);

		if(!MyQueueEmpty(&serverQueue))
		{
			p = (Packet*)MyQueueTop(&serverQueue);
			MyQueueDequeue(&serverQueue);

			pthread_mutex_unlock(&m);
			usleep(serviceTime);
		
			printf("dfdfdfdf\n");
			free(p); p = NULL;
		}

		while(!MyQueueEmpty(&serverQueue))
		{
			pthread_mutex_lock(&m);
			p = (Packet*)MyQueueTop(&serverQueue);
			MyQueueDequeue(&serverQueue);
			pthread_mutex_unlock(&m);

			usleep(serviceTime);

			printf("dfdfdfdf\n");
			free(p); p = NULL;
		}
	}
}

void* tokenThread(void* arg)
{
	int tokenIndex;
	useconds_t waitingTime, sleepTime;
	TimeVal startTime, endTime;

	waitingTime = turnRateToMicroSeconds(input.r);
	if(waitingTime > 10 * 1000000)
		waitingTime = 10 * 1000000;

	tokenIndex = 0;
	while(TRUE)
	{	

		usleep(sleepTime);
		tokenIndex++;
		pthread_mutex_lock(&m);

		
		gettimeofday(&startTime, &tz);

		if(g_tokens == input.B)
			printf("token overflow");
		else
		{
			++g_tokens;
			Packet* p = (Packet*)MyQueueTop(&arrivalQueue);
			if(p->requiredToken <= g_tokens)
			{
				g_tokens -= p->requiredToken;
				MyQueueDequeue(&arrivalQueue);
				MyQueueEnqueue(&serverQueue, (void*)p);

				if( 1 == My402ListLength(&serverQueue.list))
				{
					pthread_cond_signal(&newPacketToServer);
				}
			}

		}
		gettimeofday(&endTime, &tz);
		sleepTime = waitingTime - diff2Timeval(startTime, endTime);
		pthread_mutex_unlock(&m);
	}
}



int main (int argc, char *argv[])
{	
	//init input
	input.lambda = 0.5;
	input.mu = 0.35;
	input.r = 1.5;
	input.B = 10;
	input.P = 3;
	input.numToArrive = 20;
	input.fileName[0] = '\0';

	parseParams(argc, argv, &input);
	if(FALSE == checkInput(&input))
		exit(-1);

	//output parameters (alread loaded file)
	printf("Emulation Parameters:\n");

	if(!fptr)
	{
		printf("lambda = %.1f\n", input.lambda);
		printf("mu = %.2f\n", input.mu);
	}

	printf("r = %.1f\n", input.r);
	printf("B = %d\n", input.B);

	if(!fptr)
	{
		printf("P = %d\n", input.P);
		printf("number to arrive = %d\n", input.numToArrive);
	}
	else
		printf("tsfile = %s\n", input.fileName);

	//////////////////////////////////////////////

	// init queues
	MyQueueInit(&arrivalQueue);
	MyQueueInit(&serverQueue);
	g_tokenSize = input.B;



	gettimeofday(&emulationStartTime, &tz);
	printMiniSeconds(emulationStartTime, emulationStartTime);
	printf("emulation begins\n");
	pthread_create(&arrive_thread, 0, arriveThread, NULL);
	pthread_create(&server_thread, 0, serverThread, NULL);
	pthread_create(&token_thread, 0, tokenThread, NULL);

	//when to close the file??
	
}