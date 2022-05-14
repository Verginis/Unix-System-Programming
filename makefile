all: sniffer worker

sniffer: manager.cpp
	g++ -o sniffer manager.cpp

worker: worker.cpp
	g++ -o worker worker.cpp

execute: 
	./sniffer
clean:
	rm -f sniffer worker
	rm -f fifo_*