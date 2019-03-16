CC=g++
OPT=-std=c++17
LIB=-lpthread -lz -lstdc++fs


try:
	$(CC) src/main2.cpp src/utility.cpp src/request_handler.cpp -o src/httpc $(OPT) $(LIB)


debug:
	$(CC) src/main2.cpp src/utility.cpp src/request_handler.cpp -o src/httpc $(OPT) $(LIB) -g

clean:
	rm src/*.o