TARGET = dmodem
CXX = g++

INCLUDE := include/
LIB := lib/
SRC := src/
BIN := build/

CXXFLAGS := -g -Wall -std=c++20 
LDFLAGS  := -L$(LIB) -lrt -lm -lasound -ljack -pthread

SRCS = $(wildcard $(SRC)*.cpp)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(subst $(SRC), $(BIN), $(OBJS))

.PHONY: clean

all: $(TARGET)

$(BIN)%.o: $(SRC)%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(INCLUDE)

$(TARGET): $(OBJS)
	$(CXX) $^ $(LIB)/libportaudio.a -o $@ $(LDFLAGS)

clean:
	rm $(BIN)*.o
	rm $(TARGET)
