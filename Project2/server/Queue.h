
#ifndef QUEUE_H
#define QUEUE_H

#include "QueueItem.h"

class Queue {
public:
    Queue(int POOL_SIZE);
    virtual ~Queue();

    void place(QueueItem data);
    QueueItem obtain();
    bool isEmpty();
    bool isFull();

private:
    int POOL_SIZE;
    int start;
    int end;
    int count;
    QueueItem * data;
};

#endif /* QUEUE_H */

