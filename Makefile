all: server
server:
	gcc server.c -o server

clean:
	rm -rf server
