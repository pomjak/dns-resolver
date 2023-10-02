CXX=g++
RM=rm -f
# cc flags
CXXFLAGS := -std=c++20
# C PreProcessor flags
# -Iinclude to include .h files
CPPFLAGS = -Iinclude
CFLAGS= -O2  -Wall -Wextra -Wpedantic 
DEBUG= -g
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cc)
OBJS=dns

make:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) -o $(OBJS) $(SRC)

debug:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) $(DEBUG)-o $(OBJS) $(SRC)

clean:
	$(RM) $(OBJS)

changelog:
	git log --pretty="- %s" | head -n -2 > CHANGELOG.md