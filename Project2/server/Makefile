OBJS	= dataServer.o WorkerThreadArguments.o QueueItem.o Transfer.o Queue.o CommunicationThreadArguments.o
SOURCE	= dataServer.cpp WorkerThreadArguments.cpp QueueItem.cpp Transfer.cpp Queue.cpp CommunicationThreadArguments.cpp
HEADER	= dataServer.h WorkerThreadArguments.h QueueItem.h Transfer.h Queue.h CommunicationThreadArguments.h
OUT	= dataServer
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

dataServer.o: dataServer.cpp
	$(CC) $(FLAGS) dataServer.cpp 

WorkerThreadArguments.o: WorkerThreadArguments.cpp
	$(CC) $(FLAGS) WorkerThreadArguments.cpp 

QueueItem.o: QueueItem.cpp
	$(CC) $(FLAGS) QueueItem.cpp 

Transfer.o: Transfer.cpp
	$(CC) $(FLAGS) Transfer.cpp 

Queue.o: Queue.cpp
	$(CC) $(FLAGS) Queue.cpp 

CommunicationThreadArguments.o: CommunicationThreadArguments.cpp
	$(CC) $(FLAGS) CommunicationThreadArguments.cpp 


clean:
	rm -f $(OBJS) $(OUT)

