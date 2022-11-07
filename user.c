#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include "system.c"

void shmInitialize(key_t, key_t, key_t, key_t);
void request();
void release();

ResourceClass *array;
Clock *sys;
int *resourceData;
message *m;
int clockID;
int classID;
int resourceID;
int mID;
int claims;
int none = 0;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    //logfile
    FILE *log = fopen("logfile", "w");
    //get pid
    pid_t thisPID = getpid();
    //key declarations
    key_t keyClock = 4950;
    key_t keyClasses = 4951;
    key_t keyResourceData = 4952;
    key_t keyMsg = 4953;

    //sem declaration
    sem_t *sem = sem_open ("/sem", O_CREAT, 0777, 1 );

    shmInitialize(keyClock, keyClasses, keyResourceData, keyMsg);

    int place = (rand() % resourceData[1]) + 1;
    claims = place;
    unsigned int localClock[2] = {sys->clock[0], sys->clock[1]};
    int bound = 400;
    int action = (rand() % bound) +1;
    while(1)
    {
        msgrcv(mID, &m, sizeof(m), thisPID, 0);
        localClock[1] += action;
        request();
        sleep(10);
        release();
        m->type = 0;
        break;
    }
    fprintf(log, "Process %d is now terminating.\n", thisPID);
    msgsnd(mID, &m, sizeof(m), 0);
    exit(0);

}

void shmInitialize(key_t keyClock, key_t keyClasses, key_t keyResourceData, key_t keyMsg)
{
    clockID = shmget(keyClock, sizeof(Clock), PERMS | IPC_CREAT);
    classID = shmget(keyClasses, 20*sizeof(ResourceClass), PERMS | IPC_CREAT);
    resourceID = shmget(keyResourceData, sizeof(int)*2, PERMS | IPC_CREAT);
    mID = msgget(keyMsg, IPC_CREAT | 0666 );

    sys = (Clock *) shmat(clockID, 0, 0);
    array = (ResourceClass *) shmat(classID, 0, 0);
    resourceData = (int *) shmat(resourceID, 0, 0);

}


void request()
{
    FILE *log = fopen("logfile", "w");
    pid_t thisPID = getpid();
    int requests = 0;
    for(int k = 0; k<resourceData[1]; k++)
    {
        if (requests>=claims)
        return;
        else
        for(int l = 0; l<10; l++)
        {
            if(requests>=claims)
            return;
            else if(array[k].instance[l].id!=0)
            {
                enqueue(array[k].instance[l].pid, thisPID);
                fprintf(log, "Process %d has been enqueued for resource %d in class %d.\n", thisPID, l, k);
                requests++;
            }
            else
            break;
        }
        
    }
    for(int i = resourceData[1]; i <20; i++)
    {
        //request cap reached
        if (requests>=claims)
            return;
        //not shareable, but has been assigned
        else if(array[i].sharedStatus == false && array[i].beenShared == true)
        {   
            continue;
        }
        //not shareable, but has not been assigned
        else if(array[i].sharedStatus == false && array[i].beenShared == false)
        {
            array[i].beenShared = true;
            //loop through instances in resource class
            for(int j = 0; j<10; j++)
            {   
                if(requests>=claims)
                return;
                else if(array[i].instance[j].id!=0)
                {
                    enqueue(array[i].instance[j].pid, thisPID);
                    fprintf(log, "Process %d has been enqueued for resource %d in class %d.\n", thisPID, j, i);
                    requests++;
                }
                else
                break;
            }
        }
    }
}

void release()
{
    FILE *log = fopen("logfile", "w");
    pid_t thisPID = getpid();
    for(int k = 0; k<20; k++)
    {
        for(int l = 0; l<10; l++)
        {
            if(array[k].instance[l].pid->array[array[k].instance[l].pid->front] == thisPID)
            {
                thisPID = dequeue(array[k].instance[l].pid);
                fprintf(log, "Process %d has finished using resource %d in class %d.\n", thisPID, l, k);
            }
            else
            break;
        }
        
    }

}
