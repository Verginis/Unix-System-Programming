#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
#include <cerrno>
#include <fcntl.h>

using namespace std;

#define READ 0
#define WRITE 1
#define STDOUT 1
#define SIZE 1024

void create_listener(char* foldername, int* fd);
char* return_filename(char* path);
void create_worker(char* fifo_name);


    

class worker_info
{
private:
    /* data */
public:
    int fifo_fd;
    char fifo_name[SIZE];
    pid_t pid;
    bool isworkingnow;



    worker_info(int fifo_fd, char* f_name, pid_t pid):fifo_fd(fifo_fd), pid(pid), isworkingnow(true){
        strcpy(fifo_name, f_name);
    }
    
};


class worker_list
{
private:
    /* data */
public:
    vector < worker_info* > workers; 
    void add_w_info(worker_info* winfo){
        workers.push_back(winfo);
    }
    worker_info* get_free_worker();
    ~worker_list();
};




