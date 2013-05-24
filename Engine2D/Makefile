CC=clang
CXX=clang++
RM=rm -f
CXXFLAGS= -g -Wall -I/usr/local/include/freetype2 -I/usr/local/include
LDFLAGS= -g  -L/usr/local/lib/ -lfreetype
LDLIBS= -framework OpenGL -lglew -lglfw -lphysfs -lv8 -lsqlite3

SRCS= src/main.cpp src/JSInput.cpp src/JSDraw.cpp src/JSSys.cpp src/JSFS.cpp src/JSDatabase.cpp src/Database.cpp src/Filesystem.cpp src/Logger.cpp src/extern/GLFT_Font.cpp src/Shader.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

gdb: all
	gdb bin/Engine

run: all
	bin/Engine

all: Engine

Engine: $(OBJS)
	clang++ $(LDFLAGS) -o bin/Engine $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
