CXX := g++
CXXFLAGS := -std=c++17 -pthread -I. -Isrc

HTTP_CORE_SRC := \
	src/http/HttpContext.cpp \
	src/http/HttpRequest.cpp

COMMON_SRC := \
	src/Server.cpp \
	src/epoll.cpp \
	src/channel.cpp \
	src/mysocket.cpp \
	src/sock_addr.cpp \
	src/EventLoop.cpp \
	src/Acceptor.cpp \
	src/Connection.cpp \
	src/Buffer.cpp \
	LogStream/Logger.cpp \
	LogStream/LogStream.cpp \
	LogStream/asyncLogger.cpp \
	time/Timer.cpp \
	time/TimeQueue.cpp

HTTP_SERVER_SRC := \
	HttpServer.cpp \
	src/http/HttpServer.cpp \
	src/http/HttpResponse.cpp \
	$(COMMON_SRC) \
	$(HTTP_CORE_SRC)

CLIENT_SRC := \
	client.cpp \
	$(COMMON_SRC) \
	$(HTTP_CORE_SRC)


.PHONY: all clean

all: server #client

server: $(HTTP_SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(HTTP_SERVER_SRC) -o $@

#client: $(CLIENT_SRC)
#	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $@

clean:
	rm -f server client

