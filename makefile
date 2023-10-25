CXX = g++
CXXFLAGS = -std=c++23 -I./include --debug
LDLIBS = -lssl -lcrypto

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:.cc=.o)

all: main

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f main $(OBJS)

run: 
	./main



