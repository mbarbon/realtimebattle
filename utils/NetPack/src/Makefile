.c.o:
	g++ -c $<

remote_client: remote_client.o NetConnection.o Packets.o ClientSocket.o SocketHandler.o
	g++ -o remote_client remote_client.o NetConnection.o Packets.o ClientSocket.o SocketHandler.o
local_client: local_client.o NetConnection.o Packets.o ClientSocket.o ServerSocket.o SocketHandler.o
	g++ -o local_client local_client.o NetConnection.o Packets.o ClientSocket.o ServerSocket.o SocketHandler.o
server: server.o ServerSocket.o  NetConnection.o SocketHandler.o  Packets.o 
	g++ -o server server.o ServerSocket.o NetConnection.o SocketHandler.o Packets.o 



all: remote_client local_client server

