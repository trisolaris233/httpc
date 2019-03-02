CC=g++
OPT=-std=c++17
LIB=-lpthread -lz


httpc: src/connection_manager.o src/connection.o src/http_router.o src/request_handler.o src/utility.o src/main2.cpp
	$(CC) src/connection_manager.o src/connection.o src/http_router.o src/request_handler.o  src/main2.cpp -o src/httpc $(OPT) $(LIB)

src/utility.o: src/utility.hpp src/utility.cpp
	$(CC) -c src/utility.cpp -o src/utility.o $(OPT) $(LIB)

src/connection_manager.o: src/connection_manager.hpp src/connection_manager.cpp
	$(CC) -c src/connection_manager.cpp -o src/connection_manager.o $(OPT) $(LIB)

src/connection.o: src/connection.hpp src/connection.cpp src/connection_manager.o
	$(CC) -c src/connection.cpp -o src/connection.o $(OPT) $(LIB)
src/http_router.o: src/http_router.hpp src/http_router.cpp
	$(CC) -c src/http_router.cpp -o src/http_router.o $(OPT) $(LIB)
src/request_handler.o: src/request_handler.hpp src/request_handler.cpp
	$(CC) -c src/request_handler.cpp -o src/request_handler.o $(OPT) $(LIB)

clean:
	rm src/*.o

try:
	$(CC) src/main2.cpp src/utility.cpp src/connection.cpp src/connection_manager.cpp src/request_handler.cpp -o src/httpc $(OPT) $(LIB)


debug:
	$(CC) src/main2.cpp src/utility.cpp src/connection.cpp src/connection_manager.cpp src/request_handler.cpp -o src/httpc $(OPT) $(LIB) -g