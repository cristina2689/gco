

all:
	g++ `pkg-config --cflags opencv` -o ocv ocvtest.cpp `pkg-config --libs opencv` -L /usr/include -std=c++11
