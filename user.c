#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include "system.c"

#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

void shmInitialize(key_t, key_t, key_t);
void request(FILE *);
void release(FILE *);
void semWait();
void semPost();

static struct sembuf sop;
ResourceClass *resourceArray;
Clock *sys;
int *resourceData;

int clockID;
int classID;
int resourceID;
int claims;
int none = 0;
int semAddress;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    //logfile
    
    //get pid
    pid_t thisPID = getpid();
    char string[50] = "logfile.";
    char string1[10] = "";
    sprintf(string1, "%d", thisPID);
    strcat(string, string1);
    FILE *log = fopen(string, "w");
    //key declarations
    key_t keyClock = 4950;
    key_t keyClasses = 4951;
    key_t keyResourceData = 4952;
    key_t keySem = 4953;

    //sem declaration
    semAddress = semget(keySem, 1, PERMS | IPC_CREAT);

    shmInitialize(keyClock, keyClasses, keyResourceData);

    int place = (rand() % resourceData[1]) + 1;
    claims = place;
    unsigned int localClock[2] = {sys->clock[0], sys->clock[1]};
    int bound = 400;
    int action = (rand() % bound) +1;
    localClock[1] += action;
    if(localClock[1] >= 1000)
        {
            localClock[0] += (localClock[1] % 1000);
            localClock[1] = (localClock[1] - ((localClock[1] % 1000) * 1000));
        }

    while(1)
    {
        semWait();
        request(log);
        release(log);
        semPost();
        break;
    }
    sys->clock[1] += localClock[1];
    if(sys->clock[1] >= 1000)
    {
        sys->clock[0] += (sys->clock[1] % 1000);
        sys->clock[1] = (sys->clock[1] - ((sys->clock[1] % 1000) * 1000));
    }
    fprintf(log, "Process %d is now terminating at time: %d seconds and %d miliseconds.\n", thisPID, sys->clock[0], sys->clock[1]);
    fclose(log);
    exit(0);

}

void shmInitialize(key_t keyClock, key_t keyClasses, key_t keyResourceData)
{
    clockID = shmget(keyClock, sizeof(Clock), PERMS | IPC_CREAT);
    classID = shmget(keyClasses, 20*sizeof(ResourceClass), PERMS | IPC_CREAT);
    resourceID = shmget(keyResourceData, sizeof(int)*2, PERMS | IPC_CREAT);
    
    sys = (Clock *) shmat(clockID, 0, 0);
    resourceArray = (ResourceClass *) shmat(classID, 0, 0);
    resourceData = (int *) shmat(resourceID, 0, 0);

}


void request(FILE * log)
{
    pid_t thisPID = getpid();
    int requests = 0;
    for(int k = 0; k<resourceData[1]; k++)
    {
            for(int l = 0; l<10; l++)
            {
                if(resourceArray[k].instance[l].id!=0)
                {
                    for(int x = 0; x<18; x++)
                    {
                        if(resourceArray[k].instance[l].pidArray[x] ==0)
                        {
                            resourceArray[k].instance[l].pidArray[x] = thisPID;
                            if(resourceArray[k].instance[l].pidArray[0] == thisPID)
                            {
                                resourceArray[k].instance[l].pidUsing = thisPID;
                            }
                            break;
                        }
                    }
                      
                    fprintf(log, "Process %d has been enqueued for resource %d in class %d.\n", thisPID, l, k);
                    requests++;
                }
                else
                break;
            }
    }
    for(int i = resourceData[1]; i <20; i++)
    {
            //loop through instances in resource class
            for(int j = 0; j<10; j++)
            {  
                if(resourceArray[i].instance[j].id!=0)
                {
                    for(int x = 0; x<18; x++)
                    {
                        if(resourceArray[i].instance[j].pidArray[x] ==0)
                        {
                            resourceArray[i].instance[j].pidArray[x] = thisPID;
                            if(resourceArray[i].instance[j].pidArray[0] == thisPID)
                            {
                                resourceArray[i].instance[j].pidUsing = thisPID;
                            }
                            break;
                        }
                    }
                    
                    fprintf(log, "Process %d has been enqueued for resource %d in class %d.\n", thisPID, j, i);
                    requests++;
                }
                else
                break;
            }
     }
 }

void release(FILE *log)
{
    pid_t thisPID = getpid();
    for(int k = 0; k<20; k++)
    {
        for(int l = 0; l<10; l++)
        {
            if(resourceArray[k].instance[l].pidUsing==thisPID && resourceArray[k].instance[l].id != 0)
            {
                for(int i = 0; i < 17; i++)
                {
                    resourceArray[k].instance[l].pidArray[i] = resourceArray[k].instance[l].pidArray[i+1];
                }
                if(resourceArray[k].instance[l].pidArray[0] == 0)
                {
                    resourceArray[k].instance[l].pidUsing = 0;
                }
                
                fprintf(log, "Process %d has finished using resource %d in class %d.\n", thisPID, l, k);
            }
            else
            break;
        }
        
    }

}

void semWait()
{
    sop.sem_num = 0;
	sop.sem_op = -1;
	sop.sem_flg = 0;
    semop(semAddress, &sop, 1);
}

void semPost() {
	sop.sem_num = 0;
	sop.sem_op = 1;
	sop.sem_flg = 0;
	semop(semAddress, &sop, 1);
}
