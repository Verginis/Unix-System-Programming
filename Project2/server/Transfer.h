
#ifndef TRANSFER_H
#define TRANSFER_H

#include <cstdlib>


class Transfer {
public:
    int write_all(int fd, void *buff, size_t size) ;

    int read_all(int fd, void *buff, size_t size);
};

#endif /* TRANSFER_H */

