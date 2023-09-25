CXX=g++
RM=rm -f
CPPFLAGS= -O2 -std=c++20 -Wall -Wextra -Wpedantic 
DEBUG= -g
SRCS=src/ipkcpd.cc src/ipkcpd_fun.cc src/lexer.cc src/parser.cc src/frac.cc
OBJS=ipkcpd

make:$(SRCS)
	$(CXX) $(CPPFLAGS) -o $(OBJS) $(SRCS)

debug:$(SRCS)
	$(CXX) $(CPPFLAGS) $(DEBUG) -o $(OBJS) $(SRCS)

clean:
	$(RM) $(OBJS)

changelog:
	git log --pretty="- %s" | head -n -2 > CHANGELOG.md