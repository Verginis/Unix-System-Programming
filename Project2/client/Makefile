OBJS	= remoteClient.o Transfer.o
SOURCE	= remoteClient.cpp Transfer.cpp
HEADER	= remoteClient.h Transfer.h
OUT	= remoteClient
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

remoteClient.o: remoteClient.cpp
	$(CC) $(FLAGS) remoteClient.cpp 

Transfer.o: Transfer.cpp
	$(CC) $(FLAGS) Transfer.cpp 


clean:
	rm -f $(OBJS) $(OUT)
