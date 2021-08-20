build: server client

clean: server server.o client client.o
	rm -f server
	rm -f server.o
	rm -f client
	rm -f client.o

server: server.o
	g++ -o server server.o

server.o: server.cpp
	g++ -c server.cpp

client: client.o
	g++ -o client client.o

client.o: client.cpp
	g++ -c client.cpp
