all: server client

server: server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp
	g++ server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp -o server

client: client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp
	g++ client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp -o client

client_test: client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp
	g++ client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp -o client_test
