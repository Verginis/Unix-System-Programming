
#ifndef COMMUNICATIONTHREADARGUMENTS_H
#define COMMUNICATIONTHREADARGUMENTS_H

#include <signal.h>



class CommunicationThreadArguments {
public:
    int newsock;
    int block_size;
    class Queue * queue;
    pthread_mutex_t * client_lock;
    
    CommunicationThreadArguments(int newsock, int block_size, class Queue * queue);
    virtual ~CommunicationThreadArguments();
};

#endif /* COMMUNICATIONTHREADARGUMENTS_H */

