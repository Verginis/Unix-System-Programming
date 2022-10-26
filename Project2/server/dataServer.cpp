#include <vector>
#include <dirent.h>
#include <set>
#include <iterator>
#include <sys/stat.h>
#include <fcntl.h>

#include "dataServer.h"
#include "CommunicationThreadArguments.h"
#include "WorkerThreadArguments.h"
#include "Transfer.h"
#include "Queue.h"

using namespace std;

int sock = -1;

void perror_exit(string message) {
    perror(message.c_str());
    exit(EXIT_FAILURE);
}

set<string> list(set<string>& history, string name) {
    DIR *dp;
    struct dirent *dir;
    set<string> files;

    if ((dp = opendir(name.c_str())) == NULL) {
        return files;
    }

    while ((dir = readdir(dp)) != NULL) {
        if (dir->d_ino > 0) {
            if (dir->d_type == DT_DIR) {
                if (string(dir->d_name) != "." && string(dir->d_name) != "..") {
                    string newname = name + "/" + dir->d_name;

                    if (history.find(newname) == history.end()) {
                        history.insert(newname);
                        set<string> subdir_files = list(history, newname);

                        for (auto s : subdir_files) {
                            files.insert(s);
                        }
                    }
                }
            } else {
                string newname = name + "/" + dir->d_name;
                files.insert(newname);
            }
        }
    }
    closedir(dp);

    return files;
}

//function for communication thread
void* communication_with_client_routine(void* ptr) {
    Transfer transfer;
    CommunicationThreadArguments * args = (CommunicationThreadArguments*) ptr;
    char * block = new char[args->block_size]();

    cout << "Thread created for fd: " << args->newsock << "block size: " << args->block_size << endl;

    if (transfer.write_all(args->newsock, &args->block_size, sizeof (args->block_size)) < 0) {
        perror("write_all");
    } else {
        if (transfer.read_all(args->newsock, block, args->block_size) < 0) {
            perror_exit("read_all");
        }

        string filename = block;

        cout << "Request for directory named: " << filename << endl;


        set<string> history;
        history.insert(".");
        history.insert("..");

        set<string> files = list(history, filename);
        unsigned N = files.size();

        if (transfer.write_all(args->newsock, &N, sizeof (N)) < 0) {
            perror("write_all");
        }

        for (string s : files) {
            QueueItem item(args, s);
            args->queue->place(item);
        }

        for (unsigned int i = 0; i < files.size(); i++) {
            char byte;

            if (transfer.read_all(args->newsock, &byte, sizeof (char)) < 0) {
                perror_exit("read_all");
            }
        }
    }

    cout << "Thread destroyed for fd: " << args->newsock << endl;

    delete [] block;

    close(args->newsock);

    delete args;

    return NULL;
}

//function for the worker threads
void* worker_thread_routine(void* ptr) {
    WorkerThreadArguments * args = (WorkerThreadArguments*) ptr;
    Transfer transfer;
    char * block = new char[args->block_size]();
    struct stat file_metadata;

    while (true) {
        QueueItem item = args->queue->obtain();

        if (item.terminate == true) {
            break;
        }

        stat(item.filename.c_str(), &file_metadata);

        strcpy(block, item.filename.c_str());

        item.print();

        pthread_mutex_lock(item.args->client_lock);

        // send filename
        if (transfer.write_all(item.args->newsock, block, args->block_size) < 0) {
            perror("write_all");
        }

        // send metadata

        if (transfer.write_all(item.args->newsock, &file_metadata, sizeof (file_metadata)) < 0) {
            perror("write_all");
        }

        off_t filesize = file_metadata.st_size;
        off_t blocks = (filesize / args->block_size) + (((filesize % args->block_size) > 0) ? 1 : 0);
        off_t last_block_load = filesize % args->block_size;

        if (transfer.write_all(item.args->newsock, &blocks, sizeof (blocks)) < 0) {
            perror("write_all");
        }

        if (transfer.write_all(item.args->newsock, &last_block_load, sizeof (last_block_load)) < 0) {
            perror("write_all");
        }

        // send data
        int fd = open(item.filename.c_str(), O_RDONLY);

        if (fd > 0) {
            for (int i = 0; i < blocks; i++) {
                int current_block_size;

                if (i != blocks - 1) {
                    current_block_size = args->block_size;
                } else {
                    current_block_size = last_block_load;
                }

                if (transfer.read_all(fd, block, current_block_size) < 0) {
                    perror("read_all");
                }

                if (transfer.write_all(item.args->newsock, block, current_block_size) < 0) {
                    perror("read_all");
                }
            }
        } else {
            
        }

        close(fd);


        pthread_mutex_unlock(item.args->client_lock);
    }

    delete [] block;

    delete args;

    return NULL;
}

void signal_handler(int sig) {
    cout << "SIGINT received \n";
    close(sock);
    sock = -1;
}

int main(int argc, char * argv []) {
    int port, newsock, error;
    pthread_t thread_id;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof (client);
    int thread_pool_size, queue_size, block_size;
    struct sockaddr * serverptr = (struct sockaddr *) & server;

    struct sockaddr * clientptr = (struct sockaddr *) & client;
    //    struct hostent * rem;

    vector<pthread_t > communication_thread_ids;
    vector<pthread_t > worker_thread_ids;

    if (argc != 9) {
        printf("Please give port number\n");
        exit(1);
    }

    for (int i = 1; i < argc; i = i + 2) {
        if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv [i + 1]);
            printf("port = %d \n", port);
        }
        if (strcmp(argv[i], "-s") == 0) {
            thread_pool_size = atoi(argv [i + 1]);
            printf("thread_pool_size = %d \n", thread_pool_size);
        }
        if (strcmp(argv[i], "-q") == 0) {
            queue_size = atoi(argv [i + 1]);
            printf("queue_size = %d \n", queue_size);
        }
        if (strcmp(argv[i], "-b") == 0) {
            block_size = atoi(argv [i + 1]);
            printf("block_size = %d \n", block_size);
        }
    }

    Queue queue(queue_size);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror_exit("socket");
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    if (bind(sock, serverptr, sizeof (server)) < 0) {
        perror_exit("bind");
    }

    if (listen(sock, 5) < 0) {
        perror_exit("listen");
    }

    for (int i = 0; i < thread_pool_size; i++) {
        WorkerThreadArguments * args = new WorkerThreadArguments(&queue, block_size);

        if ((error = pthread_create(&thread_id, NULL, worker_thread_routine, (void*) args)) < 0 || sock == -1) { 
            close(sock);
            perror_exit("create thread");
        }

        worker_thread_ids.push_back(thread_id);
    }

    printf("Listening for connections to port %d \n ", port);

    signal(SIGINT, signal_handler);

    while (sock != -1) {
        if ((newsock = accept(sock, clientptr, & clientlen)) < 0 || sock == -1) {
            break;
        }

        CommunicationThreadArguments * args = new CommunicationThreadArguments(newsock, block_size, &queue);

        if ((error = pthread_create(&thread_id, NULL, communication_with_client_routine, (void*) args)) < 0 || sock == -1) { 
            perror("create thread");
            break;
        }

        communication_thread_ids.push_back(thread_id);
    }

    for (unsigned i = 0; i < worker_thread_ids.size(); i++) {
        queue.place(QueueItem(true));
    }

    for (unsigned i = 0; i < communication_thread_ids.size(); i++) {
        pthread_join(communication_thread_ids[i], NULL);
    }

    for (unsigned i = 0; i < worker_thread_ids.size(); i++) {
        pthread_join(worker_thread_ids[i], NULL);
    }

    return 0;
}