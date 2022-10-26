#include <stdio.h>   // from www.mario-konrad.ch
#include <pthread.h>
#include <unistd.h>

#include "Queue.h"
#include "QueueItem.h"


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;

Queue::Queue(int POOL_SIZE) : POOL_SIZE(POOL_SIZE) {
    start = 0;
    end = -1;
    count = 0;
    data = new QueueItem[POOL_SIZE];
}

Queue::~Queue() {
    delete [] data;
}

bool Queue::isEmpty() {
    if (count == 0) {
        return true;
    } else {
        return false;
    }
}

bool Queue::isFull() {
    if (count == POOL_SIZE) {
        return true;
    } else {
        return false;
    }
}

void Queue::place(QueueItem item) {
    pthread_mutex_lock(&mtx);
    while (isFull()) {
        pthread_cond_wait(&cond_nonfull, &mtx);
    }
    end = (end + 1) % POOL_SIZE;
    data[end] = item;
    count++;
    pthread_cond_broadcast(&cond_nonempty);

    pthread_mutex_unlock(&mtx);
}

QueueItem Queue::obtain() {
    QueueItem item;
    
    pthread_mutex_lock(&mtx);
    while (isEmpty()) {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }
    item = data[start];
    start = (start + 1) % POOL_SIZE;
    count--;
    pthread_cond_broadcast(&cond_nonfull);
    pthread_mutex_unlock(&mtx);
    
    return item;
}