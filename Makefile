CXX = g++

CXXFLAGS = -Wall -std=c++20

SRC_DIR = scr
BUILD_DIR = targets
INCLUDE_DIR = $(SRC_DIR)/headers

SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/cryption.cpp $(SRC_DIR)/execute.cpp $(SRC_DIR)/flags.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

TARGET = $(BUILD_DIR)/stegopp

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(SRC_DIR)/*.o

run: all
	./$(TARGET)
