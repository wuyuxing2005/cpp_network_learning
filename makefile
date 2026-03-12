all: server client

server: server.cpp Server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp
	g++ server.cpp Server.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp -o server

client: client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp
	g++ client.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp -o client

client_test: client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp
	g++ client_test.cpp epoll.cpp channel.cpp mysocket.cpp sock_addr.cpp EventLoop.cpp Acceptor.cpp Connection.cpp -o client_test
