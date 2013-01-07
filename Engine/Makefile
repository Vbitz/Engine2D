CC=clang
CXX=clang++
RM=rm -f
CPPFLAGS= -g -Wall -Werror
LDFLAGS= -g
LDLIBS= -framework OpenGL

SRCS= src/main.cpp src/App.cpp
OBJS=$(subst .cc,.o,$(SRCS))

run: all
	bin/Engine

all: Engine

Engine: $(OBJS)
	clang++ $(LDFLAGS) -o bin/Engine $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend