CC      = g++
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib
OPTIONS= -g
LIBRARY = -lpthread

LIB_SRC = rvm.cpp


all: node0 node1 node2 node3 node4 node5 client1 client2 client3 client4 client5

node0: 
	$(CC) $(OPTIONS) -o node0 node.cpp $(LIBRARY)

node1: 
	$(CC) $(OPTIONS) -o node1 node.cpp $(LIBRARY)

node2: 
	$(CC) $(OPTIONS) -o node2 node.cpp $(LIBRARY)

node3: 
	$(CC) $(OPTIONS) -o node3 node.cpp $(LIBRARY)

node4: 
	$(CC) $(OPTIONS) -o node4 node.cpp $(LIBRARY)

node5: 
	$(CC) $(OPTIONS) -o node5 node.cpp $(LIBRARY)


client1: 
	$(CC) $(OPTIONS) -o client1 client.cpp $(LIBRARY)

client2: 
	$(CC) $(OPTIONS) -o client2 client.cpp $(LIBRARY)

client3: 
	$(CC) $(OPTIONS) -o client3 client.cpp $(LIBRARY)

client4: 
	$(CC) $(OPTIONS) -o client4 client.cpp $(LIBRARY)

client5: 
	$(CC) $(OPTIONS) -o client5 client.cpp $(LIBRARY)




clean:
	rm -rf *.o node0 node1 node2 node2 node3 node4 node5 client1 client2 client3 client4 client5


