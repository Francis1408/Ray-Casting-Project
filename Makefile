# Makefile for GLFW + GL project with glad

# Compiler and flags
CXX = g++
CFLAGS = -fdiagnostics-color=always -g

# Directories
SRC_DIR = .
GLAD_DIR = glad

# Source files
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
GLAD_FILES = $(wildcard $(GLAD_DIR)/*.c)

# Output binary
OUT = main

# Libraries
LIBS = -lGL -lglfw

# Build target
all: $(OUT)

$(OUT): $(CPP_FILES) $(GLAD_FILES)
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS)

# Run target
run: $(OUT)
	./$(OUT)

# Clean target
clean:
	rm -f $(OUT)