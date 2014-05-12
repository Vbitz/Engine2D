# Roadmap

- **Feature Freeze**
- **DONE** - Split Application into Application and Window_GLFW
**Removing ScriptingManager from the current Roadmap**
- Split Application into Application and ScriptingManager
	- **DONE** Stub out ScriptingManager
	- Implament ScriptingManager
	- Intergrate ScriptingManager with Application
**Roadmap resumes here**
- **DONE** - Split Draw2D into RenderGL and Draw2D
	- Move AddItem and buffer management into RenderGL
	- Intergrate RenderGL and Draw2D
- Split RenderGL into RenderGL2 and RenderGL3
	- Split OGL2 and OGL3 codepaths between the 2 versions
- Remove dependancy on FreeType
	- Rewrite GLFT_Font as a Sprite based Draw2D driven class
- Remove dependancy on FreeImage
	- Improve image rendering using the Event system
- Report to Windows
	- Stub Out Platform_win.cpp
	- Use GYP to generate a VS2013 project
	- Fix linking issues and install libarys
	- Implament Platform_win.cpp
	- Update engine2D.gyp
- **Unfreeze Feature Additions**
- Add new Scripting Features
	- Implament new fucntions in javascriptAPI.txt
- Support C++ based addons
	- Basic .dll/.so/.dylib loading support
	- Engine2D.hpp common API header file
- Audio Support
	+ Add OpenAL Depend (maybe another libary, something to really look into)
	+ Write C++ Clases
	+ Write JS API
- Port to Linux
	+ Stub Out Platform_linux.cpp
	+ Use GYP to generate a Makefile
	+ Fix linking issues and record package details
	+ Implament Platform_linux.cpp
	+ Update engine2D.gyp
- Support C based addons
	+ Engine2D.h common API header file with wrapper code
- Entity System
	+ Abstract out
- UI System
- 3D Support
- ???
