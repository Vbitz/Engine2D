CC=clang
CXX=clang++
GDB=lldb
VALGRIND=valgrind
RM=rm -f
OPTIMISE=
WARNING= -Wall -Weffc++
VFLAGS=
CXXFLAGS= $(OPTIMISE) -g -std=gnu++11 -stdlib=libc++ $(WARNING) -I/usr/local/include/freetype2 -I/usr/local/include
LDFLAGS= $(OPTIMISE) -g -std=gnu++11 -stdlib=libc++ -L/usr/local/lib/ -lfreetype
LDLIBS= -framework Cocoa -framework OpenGL -framework IOKit -lglew -lglfw3 -lphysfs -lv8 -lfreeimage -lpthread

SRCS= 	src/main.cpp \
		src/JSInput.cpp \
		src/JSDraw.cpp \
		src/JSSys.cpp \
		src/JSFS.cpp \
		src/JSDatabase.cpp \
		src/JSMod.cpp \
		src/JSUnsafe.cpp \
		src/Database.cpp \
		src/Filesystem.cpp \
		src/extern/GLFT_Font.cpp \
		src/extern/jsoncpp.cpp \
		src/GL3Buffer.cpp \
		src/Shader.cpp \
		src/EngineUI.cpp \
		src/Draw2D.cpp \
		src/Logger.cpp \
		src/Profiler.cpp \
		src/FramePerfMonitor.cpp \
		src/ResourceManager.cpp \
		src/Config.cpp \
		src/LogGraphEvents.cpp \
		src/Util.cpp \
		src/PlatformOSX.cpp \
		src/Events.cpp \
		src/TestSuite.cpp \
		src/Application.cpp \
		src/PlatformTests.cpp

OBJS=$(subst .cpp,.o,$(SRCS)) src/extern/sqlite3.o
OUTPUT=bin/Engine

doc: doc/include.pdf

doc/include.pdf: doc/include.dot
	neato -Tpdf doc/include.dot > doc/include.pdf

doc/include.dot: $(SRCS)
	cinclude2dot --src src/  > doc/include.dot

modTest: src/modTest.o
	mkdir -p res/modules
	$(CXX) -shared -o res/modules/testing.dylib src/modTest.o

valgrind: all
	sudo $(VALGRIND) $(VFLAGS) $(OUTPUT)

gdb: all
	$(GDB) $(OUTPUT)

run: all
	$(OUTPUT) -devmode -debug

test: all
	$(OUTPUT) -test # tests run in normal conditions
		# some tests are only enabled in debug mode but those are special cases

all: Engine

Engine: $(OBJS) src/extern/sqlite3.o
	mkdir -p bin
	$(CXX) $(LDFLAGS) -o $(OUTPUT) $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

release: all
	rm -rf release-osx

	mkdir -p release-osx
	mkdir -p release-osx/Engine.app
	mkdir -p release-osx/Engine.app/Contents
	mkdir -p release-osx/Engine.app/Contents/MacOS
	mkdir -p release-osx/Engine.app/Contents/Resources/res
	mkdir -p release-osx/Engine.app/Contents/Resources/res/config
	mkdir -p release-osx/Engine.app/Contents/Resources/res/script
	mkdir -p release-osx/Engine.app/Contents/Resources/res/image

	cp -R res/lib release-osx/Engine.app/Contents/Resources/res/lib
	cp -R res/fonts release-osx/Engine.app/Contents/Resources/res/fonts
	cp -R res/shaders release-osx/Engine.app/Contents/Resources/res/shaders
	cp Info.plist release-osx/Engine.app/Contents
	cp res/config/config.json release-osx/Engine.app/Contents/Resources/res/config/config.json
	cp res/script/test.js release-osx/Engine.app/Contents/Resources/res/script/test.js
	cp res/image/testing.png release-osx/Engine.app/Contents/Resources/res/image/testing.png
	cp bin/Engine release-osx/Engine.app/Contents/MacOS/Engine
	cp run.sh release-osx/Engine.app/Contents/MacOS/run.sh

	chmod +x release-osx/Engine.app/Contents/MacOS/run.sh

	cp /usr/local/lib/libGLEW.1.10.0.dylib release-osx/Engine.app/Contents/MacOS
	cp /usr/local/lib/libphysfs.2.0.3.dylib release-osx/Engine.app/Contents/MacOS
	cp /usr/local/lib/libv8.dylib release-osx/Engine.app/Contents/MacOS
	cp /usr/local/lib/libfreeimage-3.15.4.dylib release-osx/Engine.app/Contents/MacOS
	cp /usr/local/lib/libfreetype.6.dylib release-osx/Engine.app/Contents/MacOS

	install_name_tool -change /usr/local/lib/libGLEW.1.10.0.dylib @executable_path/libGLEW.1.10.0.dylib release-osx/Engine.app/Contents/MacOS/Engine
	install_name_tool -change /usr/local/lib/libphysfs.2.0.3.dylib @executable_path/libphysfs.2.0.3.dylib release-osx/Engine.app/Contents/MacOS/Engine
	install_name_tool -change /usr/local/lib/libfreeimage-3.15.4.dylib @executable_path/libfreeimage-3.15.4.dylib release-osx/Engine.app/Contents/MacOS/Engine
	install_name_tool -change /usr/local/lib/libv8.dylib @executable_path/libv8.dylib release-osx/Engine.app/Contents/MacOS/Engine
	install_name_tool -change /usr/local/lib/libfreetype.6.dylib @executable_path/libfreetype.6.dylib release-osx/Engine.app/Contents/MacOS/Engine

include .depend
