#include "manager.h"

worker_list wlist;
int fifo_counter = 1;

worker_info* worker_list::get_free_worker(){
    for(worker_info* i : workers){
        if(!i->isworkingnow){
            return i;
        }
    }
    return NULL;
}

worker_list::~worker_list()
{
    for(worker_info* i : workers){
        delete i;
    }

}

//Create listener who calls the inotifywait
void create_listener(char* dirname, int* fd){
    close(fd[READ]);
    dup2(fd[WRITE], STDOUT);
    execl("/bin/inotifywait", "/bin/inotifywait", "-m", "-e", "create", "-e", "moved_to", "-m", "--exclude", ".out", dirname, (char*) NULL);

    cout << "execl failed" << endl;
    exit(2);
}

//Takes the path that was given and returns only the name of the file
char* return_filename(char* path){
    char* tok = strtok(path, " ");
    tok = strtok(NULL, " ");
    tok = strtok(NULL, "\n");
    return tok;
}

//Create the worker
void create_worker(char* fifo_name){
    execl("worker", "worker", fifo_name, (char*) NULL);

}

//When we get the Ctrl+C signal unlinks the named pipes
void signal_handler(int sig){
    signal(SIGINT, signal_handler);
    char buffer[SIZE];

    
    for (int i = 0; i < fifo_counter; i++)
    {
        sprintf(buffer, "fifo_%d", i);
        unlink(buffer);
    }
    
    exit(0);
}





int main(int argc, char** argv){

    char dirname[SIZE], filename[SIZE], fifo_name[SIZE], path_file[2*SIZE+1];
    char slash[] = "/";
    int rsize; // how many byte read from the pipe
    pid_t pid;
    int fd[2];    
    worker_info* winfo;   
    int fifo_fd;


    signal(SIGINT, signal_handler);


    //check if there is path given from the command line and put the path in dirname
    if(argc == 3){
        if(!strcmp(argv[1], "-p")){
            strcpy(dirname, argv[2]);
        }
    }else{
        strcpy(dirname, ".");
    }

    //create a pipe for the listener
    if(pipe(fd) == -1){
        cout << "can't create pipe" << endl;
        exit(1);
    }

    //create a child process for the listener
    if((pid = fork()) < 0){
        cout << "fork failed" << endl;
        exit(3);
    }

    if(pid == 0){
        //create the listener to call the inotifywait
        create_listener(dirname, fd);
    }

    close(fd[WRITE]);
    for(;;){
        if((rsize = read(fd[READ], filename, SIZE)) < 0){
            cout << "pipe failed" << endl;
            exit(4);
        }

        strcpy(filename, return_filename(filename));
        //cout << "filename tok " << filename  << endl;
        sprintf(path_file, "%s%s%s",dirname,slash,filename);
        
        winfo = wlist.get_free_worker();
        if(winfo == NULL){

            //create named pipe for the worker
            sprintf(fifo_name, "fifo_%d",fifo_counter++);
            if(mkfifo(fifo_name, 0666) == -1){
                if(errno != EEXIST){
                    cout << "named pipe failed" << endl;
                    exit(5);
                }
            }

            

            if((pid = fork()) < 0){
                cout << "fork failed" << endl;
                exit(3);
            }

            if(pid == 0){
                
                create_worker(fifo_name);
            }

            
            //add worker to the list
            wlist.add_w_info(new worker_info(fifo_fd, fifo_name, pid));

            fifo_fd = open(fifo_name, O_WRONLY);

            if(fifo_fd < 0 ){
                cout << "named pipe failed" << endl;
                exit(6);
            }
            

            //send the path of the file to the worker
            write(fifo_fd, path_file, strlen(path_file)+1);



        }else{
            winfo->isworkingnow = true;

            write(winfo->fifo_fd, path_file, strlen(path_file)+1);

        }

        



    }
    



}



