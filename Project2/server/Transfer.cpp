#include <unistd.h>

#include "Transfer.h"

int Transfer::write_all(int fd, void *buff, size_t size) {
    size_t sent;
    int n;
    
    for (sent = 0; sent < size; sent += n) {
        if ((n = write(fd, ((char*)buff) + sent, size - sent)) == -1) {
            return -1; /* error */
        }
    }
    return sent;
}

int Transfer::read_all(int fd, void *buff, size_t size) {
    size_t sent;
    int n;

    for (sent = 0; sent < size; sent += n) {
        if ((n = read(fd, ((char*)buff) + sent, size - sent)) == -1) {
            return -1; /* error */
        }
    }
    return sent;
}

