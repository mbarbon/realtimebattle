.c.o:
	g++ -c $<

remote_client: remote_client.o NetConnection.o Packets.o ClientSocket.o
	g++ -o remote_client remote_client.o NetConnection.o Packets.o ClientSocket.o
local_client: local_client.o NetConnection.o Packets.o ClientSocket.o ServerSocket.o
	g++ -o local_client local_client.o NetConnection.o Packets.o ClientSocket.o ServerSocket.o
server: server.o ServerSocket.o  NetConnection.o
	g++ -o server server.o ServerSocket.o NetConnection.o



all: remote_client local_client server

