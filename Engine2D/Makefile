CC=clang
CXX=clang++
RM=rm -f
CPPFLAGS= -g -Wall -Werror `freetype-config --libs`
LDFLAGS= -g
LDLIBS= -framework OpenGL -lglew -lglfw `freetype-config --libs --cflags` -lphysfs -lv8

SRCS= src/main.cpp src/extern/GLFT_Font.cpp src/JsDraw.cpp src/JsSys.cpp
OBJS=$(subst .cc,.o,$(SRCS))

gdb: all
	cd bin;gdb Engine

run: all
	cd bin;./Engine

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
