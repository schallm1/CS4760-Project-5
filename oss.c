#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include "system.c"

//prototype
void shmInitialize(key_t, key_t, key_t);
void initializeClasses();
void shmDelete();

ResourceClass *resourceArray;
Clock *sys;
int *resourceData;

int clockID;
int classID;
int resourceID;
int semAddress;

#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)



int main()
{
    pid_t pid = getpid();
    pid_t wpid;
    //logfile
    FILE *log = fopen("logfile", "w");
    //key declarations
    key_t keyClock = 4950;
    key_t keyClasses = 4951;
    key_t keyResourceData = 4952;
    key_t keySem = 4953;
    //sem declaration
    int sem;
    perror("before sem:");
    semAddress = semget(keySem, 1, IPC_CREAT | PERMS);
    perror("sem: ");
    sem = semctl(semAddress, 0, SETVAL, 1);

    //variables
    int processCount = 0;
    int status = 0;

    srand(time(0));

    shmInitialize(keyClock, keyClasses, keyResourceData);
    initializeClasses();
        sys->clock[1]+= (rand() % 500) +1;
        if(sys->clock[1] >= 1000)
        {
            sys->clock[0] += (sys->clock[1] % 1000);
            sys->clock[1] = (sys->clock[1] - ((sys->clock[1] % 1000) * 1000));
        }
        if((pid = fork()) == 0)
        {
            execl("./user", "user", NULL);
        }
        fprintf(log, "Process %d has been dispatched for execution.\n", pid);

    while ((wpid = wait(&status)) > 0);
    shmDelete();
    semctl(semAddress, 0, IPC_RMID);
    fprintf(log, "Parent process is now terminating.\n");
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

    //number of resource instances
    resourceData[0] = 0;
    //number of shared resources
    resourceData[1] = 0;

    sys->clock[0] = 0;
    sys->clock[1] = 0;

}

void shmDelete()
{
    shmdt(sys);
    shmdt(resourceArray);
    shmdt(resourceData);
    shmctl(clockID, IPC_RMID, NULL);
    shmctl(classID, IPC_RMID, NULL);
    shmctl(resourceID, IPC_RMID, NULL);
}

void initializeClasses()
{
    resourceData[1] = (rand() % 3) +3;
    //loop through shareable resource classes
    for(int i =0; i < resourceData[1]; i++)
    {
        resourceArray[i].sharedStatus = true;
        resourceArray[i].beenShared = false;

        //populate each resource class with random number of instances
        int pop = (rand() % 10) + 1;
        for(int j = 0; j< pop; j++)
        {
            resourceArray[i].instance[j].id = j+1;
            resourceArray[i].instance[j].pid = createQueue(18);
            resourceData[0]++;
        }
        //fill up the rest of instance array with nullable values
        for(int k = pop; k<10; k++)
        {
            resourceArray[i].instance[k].id = 0;
        }
    }
    //loop through non shareable resource classes
    for(int x = resourceData[1]; x <20; x++)
    {
        resourceArray[x].sharedStatus = false;
        resourceArray[x].beenShared = false;

        //populate each resource class with random number of instances
        int pops = (rand() % 10) + 1;
        for(int y = 0; y < pops; y++)
        {
            resourceArray[x].instance[y].id = y;
            resourceArray[x].instance[y].pid = createQueue(18);
            resourceData[0]++;
        }
        //fill up the rest of instance array with nullable values;
        for(int z = pops; z<10; z++)
        {
            resourceArray[x].instance[z].id = 0;
        }
    }

}
