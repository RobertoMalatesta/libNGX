CC = g++
INCLUDE= ../../include

SRC = ./Test.cpp
SRC += ./MemBlockTest.cpp ../../src/Core/MemBlock.cpp

BuildTests :
	$(CC) $(SRC) -I $(INCLUDE) -o Tests

all: BuildTests

clean:
	rm ./Tests
