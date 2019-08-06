
#CXX = clang++
CXXFLAGS = -std=c++11 -I.. -Wall -Wextra -pthread

all : test
	./test

test : test.cc threadpool.h Makefile
	$(CXX) -o test $(CXXFLAGS) test.cc

clean:
	rm test
