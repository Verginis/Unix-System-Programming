#include <condition_variable>

#include "CommunicationThreadArguments.h"

CommunicationThreadArguments::CommunicationThreadArguments(int newsock, int block_size, Queue * queue) : newsock(newsock), block_size(block_size), queue(queue) {
    client_lock = new pthread_mutex_t;
    
    pthread_mutex_init(client_lock, NULL);
}

CommunicationThreadArguments::~CommunicationThreadArguments() {
    pthread_mutex_destroy(client_lock);
    delete client_lock;
}

