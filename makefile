all: server client

server: server.cpp epoll.cpp mysocket.cpp sock_addr.cpp
	g++ server.cpp epoll.cpp mysocket.cpp sock_addr.cpp -o server

client: client.cpp epoll.cpp mysocket.cpp sock_addr.cpp
	g++ client.cpp epoll.cpp mysocket.cpp sock_addr.cpp -o client
