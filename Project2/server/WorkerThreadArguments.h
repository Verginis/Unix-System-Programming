#ifndef WORKERTHREADARGUMENTS_H
#define WORKERTHREADARGUMENTS_H

#include "Queue.h"


class WorkerThreadArguments {
public:
    Queue * queue;
    int block_size;;
    
    WorkerThreadArguments(Queue * queue,  int block_size);
    virtual ~WorkerThreadArguments();

};

#endif /* WORKERTHREADARGUMENTS_H */

