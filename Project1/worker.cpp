#include "worker.h"


long int findSize(char filename[])
{
    // opening the file in read mode
    FILE* fp = fopen(filename, "r");
  
    // checking if the file exist or not
    if(fp == NULL){
        cout << "Could not open the file" << endl;
    }
  
    fseek(fp, 0L, SEEK_END);
  
    // calculating the size of the file
    long int res = ftell(fp);
  
    // closing the file
    fclose(fp);
  
    return res;
}






int main(int argc, char** argv){

    char filename[SIZE],fifo_name[SIZE],out[SIZE];
    int fifo_fd;
    int rsize;
    char* token;
    char* token2;

    if(argc == 2){
        strcpy(fifo_name, argv[1]);
        
    }

    //cout << "fifo worker"<< fifo_name <<endl;
    fifo_fd = open(fifo_name, O_RDONLY);
    
    while (true)
    {
        rsize = read(fifo_fd, filename, SIZE);
        //cout << "worker file = " << filename <<endl;

        //find the size of the file
        long int res = findSize(filename);

        //cout << "worker res " << res <<endl;

        char* buffer = new char[res];

        //open file
        FILE* fp = fopen(filename, "r");

        //copy the file into the buffer
        size_t newLen = fread(buffer, sizeof(char), res, fp);

        //vector that we put the urls
        vector < char* > urls;

        //in the code below we cut every single word of the buffer and then we check if is starting
        //with http://www. or http:// and we keep only the domain accordingly e.g.(from http://www.home.pl/airport to home.pl/airport)
        token = strtok(buffer, " ");
        
        // walk through other tokens 
        while( token != NULL ) {
            if(token2 = strstr(token, "http://www.")){
                //put the url in the vector
                urls.push_back(token2 + 11); 
                
            }else if(token2 = strstr(token, "http://")){
                //put the url in the vector
                urls.push_back(token2 + 7);
            }
            
            token = strtok(NULL, " ");
            
        }

        //keep only the domain name of the url e.g.(from home.pl/airport to home.pl)
        for( int i=0; i < urls.size(); i++){
            urls.at(i) = strtok(urls.at(i), "/");
            //cout<< urls.at(i)<<endl;
        }


        fclose(fp);

        strcat(filename,".out");
        

        ofstream file;
	    file.open(filename);



        // Create a map to store the frequency of each element in vector
        map<string, int> countMap;
        // Iterate over the vector and store the frequency of each element in map
        for (auto & elem : urls)
        {
            auto result = countMap.insert(std::pair<std::string, int>(elem, 1));
            if (result.second == false)
                result.first->second++;
        }

        // Iterate over the map
        for (auto & elem : countMap)
        {
            // If frequency count is greater than 1 then its a duplicate element
            if (elem.second > 1)
            {
                file << elem.first << " "<< elem.second << endl;
                
            }else if (elem.second == 1)
            {
                file << elem.first << endl;
            }
            
        }


	    file.close();

        //kill(getpid(),SIGSTOP);
    }
    



         

}