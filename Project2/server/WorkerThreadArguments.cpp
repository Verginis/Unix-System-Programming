#include "WorkerThreadArguments.h"

WorkerThreadArguments::WorkerThreadArguments(Queue * queue, int block_size) : queue(queue), block_size(block_size) {

}

WorkerThreadArguments::~WorkerThreadArguments() {

}

