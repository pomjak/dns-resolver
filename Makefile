CXX=g++
RM=rm -rf
# cc flags
CXXFLAGS := -std=c++20
# C PreProcessor flags
# -Iinclude to include .h files
CPPFLAGS = -Iinclude
CFLAGS= -O2  -Wall -Wextra -Wpedantic 
DEBUG= -g
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.cc)
TEST=python3 test.py
OBJS=dns

.PHONY: clean test

make:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) -o $(OBJS) $(SRC)
	

debug:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) $(DEBUG)-o $(OBJS) $(SRC)

clean:
	$(RM) $(OBJS) test-build
	$(RM) vgcore.*
test:
	rm -rf       	test-build/
	mkdir 			test-build/
	cp -r src/* 	test-build/
	cp -r include/* test-build/
	cp -r test/* 	test-build/
	cp Makefile  	test-build/
	cd test-build && \
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) *.cc -o test &&\
	$(TEST)

changelog:
	git log --pretty="- %s" | head -n -2 > CHANGELOG.md