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
TEST=python3 test.py
OBJS=dns

.PHONY: clean test

make:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) -o $(OBJS) $(SRC_DIR)/*.cc
	

debug:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) $(DEBUG)-o $(OBJS) $(SRC)

clean:
	$(RM) $(OBJS) test-build test-artifacts
	$(RM) vgcore.*

build:clean
	mkdir 			build/
	cp -r src/* 	build/
	cp -r include/* build/
	cp -r test/* 	build/
	cp Makefile  	build/
	cd build && \
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) *.cc -o dns 
	cd ..
	cp build/dns dns
	
test:clean
	mkdir 			test-build/
	mkdir 			test-artifacts/
	cp -r src/* 	test-build/
	cp -r include/* test-build/
	cp -r test/* 	test-build/
	cp Makefile  	test-build/
	cd test-build && \
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) *.cc -o test &&\
	$(TEST)

changelog:
	git log --pretty="- %s" | head -n -2 > CHANGELOG.md