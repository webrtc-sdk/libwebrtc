OBJ_DIR := obj
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include
DEPENDENCY_DIR := dependency

CXX := clang++
CXXFLAGS := -std=c++11 -g -Wall -I./$(INCLUDE_DIR) -DDEBUG -I../include
#编辑此处，将其改为libwebrtc的路径位置
LDLIBS := -L../../out-release/ -lwebrtc

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
HDRS := $(wildcard $(INCLUDE_DIR)/*.h)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPENDENCIES:= $(patsubst $(SRC_DIR)/%.cpp,$(DEPENDENCY_DIR)/%.mk,$(SRCS))
TARGET := $(BIN_DIR)/main

.DEFAULT_GOAL := $(TARGET)

include $(DEPENDENCIES)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(DEPENDENCY_DIR)/%.mk: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEPENDENCY_DIR)
	@set -e; rm -f $@; \
		$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
		sed -E 's,($*)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

.PHONY: clean
clean:
	rm -rf $(OBJS)
	rm -rf $(DEPENDENCIES)
