//Geeks for Geeks queue

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

typedef struct {
    int front, rear, size;
    int array[18];
} Queue;
 
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
Queue createQueue(Queue queue)
{
    for(int i =0; i<18; i++)
    {
        queue.array[i] = 0;
    }
    queue.front = queue.size = 0;
 
    // This is important, see the enqueue
    queue.rear = -1;
    return queue;
}
 
// Queue is full when size becomes
// equal to the capacity
int isFull(Queue queue)
{
    return (queue.size == 18);
}
 
// Queue is empty when size is 0
int isEmpty(Queue queue)
{
    return (queue.size == 0);
}
 
// Function to add an item to the queue.
// It changes rear and size
void enqueue(Queue queue, int item)
{
    if (isFull(queue))
        return;
    queue.size +=1;
    queue.rear++;
    queue.array[queue.rear] = item;
    printf("%d enqueued to queue\n", item);
}
 
// Function to remove an item from queue.
// It changes front and size
int dequeue(Queue queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    queue.size = queue.size - 1;
    int item = queue.array[queue.front];
    queue.front = (queue.front + 1)
                   %  18;
    queue.size = queue.size - 1;
    return item;
}