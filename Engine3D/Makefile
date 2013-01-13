CC=clang
CXX=clang++
RM=rm -f
CPPFLAGS= -g -Wall -Werror
LDFLAGS= -g
LDLIBS= -framework OpenGL -lglew -lglfw -lphysfs

SRCS= src/main.cpp src/App.cpp src/Display_GLFW.cpp src/Render.cpp src/RenderModel.cpp src/Shader.cpp src/Logger.cpp src/Filesystem.cpp
OBJS=$(subst .cc,.o,$(SRCS))

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
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
