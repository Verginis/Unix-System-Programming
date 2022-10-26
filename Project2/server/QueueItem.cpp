#include <iostream>

#include "QueueItem.h"
#include "CommunicationThreadArguments.h"

using namespace std;

QueueItem::QueueItem() : terminate(false) {
    args = nullptr;
    filename = "";
}

QueueItem::QueueItem(bool terminate) : terminate(terminate) {
    args = nullptr;
    filename = "";
}

QueueItem::QueueItem(CommunicationThreadArguments * args, string filename) : args(args), filename(filename), terminate(false) {

}

QueueItem::~QueueItem() {

}

void QueueItem::print() {
    cout << " service: client " << args->newsock << ", file: " << filename << ", blocksize: " << args->block_size << endl;
}