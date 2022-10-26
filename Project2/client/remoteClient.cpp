#include <sys/stat.h>
#include <limits.h>
#include <string>
#include <errno.h>
#include <fcntl.h>

#include "remoteClient.h"
#include "Transfer.h"

void perror_exit(string message) {
    perror(message.c_str());
    exit(EXIT_FAILURE);
}

//function that creates a path recursively
static bool mkdir_recursive(string dir, bool filename_postfix = true) {
    char buffer[PATH_MAX];

    snprintf(buffer, sizeof (buffer), "%s", dir.c_str());

    if (buffer[strlen(buffer) - 1] == '/') {
        buffer[strlen(buffer) - 1] = 0;
    }

    for (char * p = buffer + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            if (strcmp(buffer, "..") == 0) {
                *p = '/';
                continue;
            }

            int length = strlen(buffer);

            if (buffer[length - 1] == '.' && buffer[length - 2] == '.') {
                *p = '/';
                continue;
            }

            if (mkdir(buffer, 0755) < 0) {
                if (errno != EEXIST) {
                    cerr << "Directory: '" << buffer << "' creation failed " << endl;
                    return false;
                }
            }
            *p = '/';
        }
    }

    if (!filename_postfix) {
        if (mkdir(buffer, 0755) < 0) {
            if (errno != EEXIST) {
                cerr << "Directory: '" << buffer << "' creation failed " << endl;
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char * argv[]) {
    int port, sock;
    struct sockaddr_in server;
    struct sockaddr * serverptr = (struct sockaddr *) & server;
    struct hostent * rem;
    string ip;
    string filename;
    int block_size = 0;
    struct stat file_metadata;
    char byte = 1;

    Transfer transfer;

    if (argc != 7) {
        printf(" Please give host name and port number \n ");
        exit(1);
    }

    for (int i = 1; i < argc; i = i + 2) {
        if (strcmp(argv[i], "-i") == 0) {
            ip = argv [i + 1];
            printf("ip = %s \n", ip.c_str());
        }
        if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv [i + 1]);
            printf("port = %d \n", port);
        }
        if (strcmp(argv[i], "-d") == 0) {
            filename = argv [i + 1];
            if (filename.length() > 0 && filename[filename.length() - 1] == '/') {
                filename = filename.substr(0, filename.length() - 1);
            }

            printf("filename = %s \n", filename.c_str());
        }
    }

    if (filename[0] == '/') {
        cout << "Error: provided filepath is an absolute path. Please provide a relative path instead \n";
        exit(1);
    }

    if (filename == "") {
        cerr << "Error: missing filepath from command line. Please provide a relative path \n";
        exit(1);
    }




    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror_exit(" socket ");
    }

    if ((rem = gethostbyname(ip.c_str())) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);

    if (connect(sock, serverptr, sizeof (server)) < 0) {
        perror_exit("connect");
    }

    if (transfer.read_all(sock, &block_size, sizeof (block_size)) < 0) {
        perror_exit("read_all");
    }

    printf("Connected to %s port %d with communication block_size: %d \n", ip.c_str(), port, block_size);


    char * block = new char[block_size]();

    strcpy(block, filename.c_str());

    if (transfer.write_all(sock, block, block_size) < 0) {
        perror_exit("read_all");
    }

    unsigned N;

    if (transfer.read_all(sock, &N, sizeof (N)) < 0) {
        perror_exit("read_all");
    }

    for (unsigned int i = 0; i < N; i++) {
        // receive filename
        if (transfer.read_all(sock, block, block_size) < 0) {
            perror_exit("read_all");
        }

        filename = block;

        cout << "Filename " << (1 + i) << " of " << N << ": " << filename << endl;

        // receive metadata

        if (transfer.read_all(sock, &file_metadata, sizeof (file_metadata)) < 0) {
            perror("write_all");
        }

        cout << "Metadata: " << endl;

        cout << "\t File created at  : " << ctime(& file_metadata.st_ctime);
        cout << "\t File modified at : " << ctime(& file_metadata.st_mtime);

        off_t blocks;
        off_t last_block_load;
        
        if (transfer.read_all(sock, &blocks, sizeof (blocks)) < 0) {
            perror("read_all");
        }
        
        if (transfer.read_all(sock, &last_block_load, sizeof (last_block_load)) < 0) {
            perror("read_all");
        }
        
        off_t filesize = blocks * block_size - (block_size-last_block_load);
        
        cout << "\t Total blocks     : " << blocks << endl;
        cout << "\t Last block load  : " << last_block_load << endl;
        cout << "\t File size        : " << filesize << " bytes " << endl;
        
        
        // mkdir

        if (mkdir_recursive(filename, true) == true) {
            // receive data      
            byte = 1;

            int fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
            
            if (fd > 0) {
                for (int i = 0; i < blocks; i++) {
                    int current_block_size;

                    if (i != blocks - 1) {
                        current_block_size = block_size;
                    } else {
                        current_block_size = last_block_load;
                    }

                    if (transfer.read_all(sock, block, current_block_size) < 0) {
                        perror("read_all");
                    }

                    if (transfer.write_all(fd, block, current_block_size) < 0) {
                        perror("read_all");
                    }
                }
            }
            
            close(fd);
        } else {
            cerr << "Directory not created \n";
            
            byte = 0;
        }

        // send ACK
        if (transfer.write_all(sock, &byte, sizeof (byte)) < 0) {
            perror_exit("read_all");
        }
    }

    delete [] block;

    close(sock);

    return 0;

}