CXX := g++
CXXFLAGS := -std=c++17 -pthread -I. -Isrc

SERVER_SRC := \
	server.cpp \
	src/Server.cpp \
	src/epoll.cpp \
	src/channel.cpp \
	src/mysocket.cpp \
	src/sock_addr.cpp \
	src/EventLoop.cpp \
	src/Acceptor.cpp \
	src/Connection.cpp \
	src/Buffer.cpp

CLIENT_SRC := \
	client.cpp \
	src/Connection.cpp \
	src/EventLoop.cpp \
	src/channel.cpp \
	src/epoll.cpp \
	src/mysocket.cpp \
	src/sock_addr.cpp \
	src/Buffer.cpp

TEST_SRC := \
	test.cpp \
	src/Connection.cpp \
	src/EventLoop.cpp \
	src/channel.cpp \
	src/epoll.cpp \
	src/mysocket.cpp \
	src/sock_addr.cpp \
	src/Buffer.cpp

.PHONY: all clean

all: server client test

server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $@

client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $@

test: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $@

clean:
	rm -f server client test
