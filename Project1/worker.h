#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include <map> 
#include <fstream>


#define SIZE 1024


using namespace std;




long int findSize(char file_name[]);