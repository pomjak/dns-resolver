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

make:$(SRC) clean
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) -o $(OBJS) $(SRC_DIR)/*.cc
	
tar:clean
	tar -cvf xpomsa00.tar CHANGELOG.md include LICENSE Makefile manual.pdf README.md src test

debug:$(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) $(DEBUG)-o $(OBJS) $(SRC)

clean:
	@$(RM) $(OBJS) test-build test-artifacts
	@$(RM) vgcore.* xpomsa00.tar
	
test:clean
	@mkdir 			test-build/
	@mkdir 			test-artifacts/
	@cp -r src/* 	test-build/
	@cp -r include/* test-build/
	@cp -r test/* 	test-build/
	@cp Makefile  	test-build/
	cd test-build && \
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) *.cc -o dns &&\
	$(TEST)
	@cd ..
	@cp test-build/dns dns
	@rm -rf test-build/
	@echo "--------------------------------------------------------------"
	@echo "!!!the test results can be viewed in test-artifacts/ folder!!!"
	@echo "--------------------------------------------------------------"

changelog:
	git log --pretty="- %s" | head -n -2 > CHANGELOG.md