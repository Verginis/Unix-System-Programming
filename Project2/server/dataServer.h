
#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* ge th os tb ya dd r */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <signal.h>
#include <iostream>
#include <pthread.h>
#include <string.h>

using namespace std;


void perror_exit ( string message );

void* communication_with_client_routine(void*);

set<string> list(set<string>& history, string name);

void* worker_thread_routine(void* ptr);

void signal_handler(int sig);