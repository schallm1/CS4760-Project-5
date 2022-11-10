#include <stdbool.h>
#include <sys/sem.h>

#ifndef SYSTEM_H
#define SYSTEM_H

#define MAX 10

typedef struct
{
    int id;
    int pidUsing;
    int pidArray[18];

}Resource;

typedef struct
{
    Resource instance[MAX];
    bool sharedStatus;
    bool beenShared;
    int classID;
    
}ResourceClass;

typedef struct 
{
    unsigned int clock[2];

}Clock;

typedef struct{
    long type;
    char msgString[100];
    int index;
    int quantUsed;
    int pid;		// Store the sending process's pid.

}message;


#endif
