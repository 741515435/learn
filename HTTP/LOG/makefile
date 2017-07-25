CC = gcc
CXX = g++
CFLAGS = -Wall -Wno-unused-result

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

