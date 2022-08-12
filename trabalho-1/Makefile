# Richard Fernando Heise Ferreira (GRR20191053)

CFLAGS  = -g -Wall
CC = gcc 

#-----------------------------------------------------------------------------#
all : client server

rClient: client
	sudo ./client

rServer: server
	sudo ./server

client: client.o RawSocket.o

server: server.o RawSocket.o

#-----------------------------------------------------------------------------#

clean :
	$(RM) *.o

#-----------------------------------------------------------------------------#

purge:
	$(RM) client server *.o