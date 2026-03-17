all: server client test

server: server.cpp Server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp Buffer.cpp
	g++ -std=c++17 -pthread server.cpp Server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp Buffer.cpp -o server

client: client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp Buffer.cpp
	g++ -std=c++17 -pthread client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp Buffer.cpp -o client

client_test: client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp
	g++ -std=c++17 -pthread client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp -o client_test

test: test.cpp mysocket.cpp sock_addr.cpp
	g++ -std=c++17 -pthread test.cpp mysocket.cpp sock_addr.cpp -o test
