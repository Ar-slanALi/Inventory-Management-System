CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = inventory_oop

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

single:
	$(CXX) -std=c++11 single_file/ims_oop_unified.cpp -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe src/*.o

.PHONY: all single clean
