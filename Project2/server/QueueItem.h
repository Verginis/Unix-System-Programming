#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <string>

#include "CommunicationThreadArguments.h"

using namespace std;

class QueueItem {
public:
    QueueItem();
    QueueItem(bool terminate);
    QueueItem(CommunicationThreadArguments * args, string filename);
    virtual ~QueueItem();
    
    void print();
    
    CommunicationThreadArguments * args;
    string filename;
    bool terminate;
};

#endif /* QUEUEITEM_H */

