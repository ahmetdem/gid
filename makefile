# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include

# Libraries
LDLIBS = -lssl -lcrypto

# Source directory and object files
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:.cc=.o)

# Main executable target
all: gid

# Build the executable
gid: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDLIBS)

# Compile individual object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up object files and executable
clean:
	rm -f $(OBJS) gid

# Remove all generated files, including the directory
remove:
	rm -rf .gid

# Run the executable
run:
	./gid

# Test targets (optional)
test:
	# Add commands to run your tests here

# Help message
help:
	@echo "Available targets:"
	@echo "  all       - Build the main executable (gid)"
	@echo "  main      - Build only the main object file"
	@echo "  clean     - Remove object files and the executable"
	@echo "  remove    - Remove all generated files"
	@echo "  run       - Run the executable"
	@echo "  test      - Run unit tests (if implemented)"
	@echo "  help      - Display this help message"

.PHONY: clean remove test help
