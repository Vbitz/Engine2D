# Task List

These are not necessarily in order that they will be completed.
Most of these come from my internal task list so don't take this as final.

- Finish new Script Binding systems
	- Finish adding new functions to api.js
	- Add support for JS Classes
	- Add support for variable arguments
- Native Window Contexts ??
	- Add WindowOSX
	- Add WindowLinux
	- Add WindowWin
- Autoset configDir based on filename
	- You should be able to override this in the configs
- Change logging to a rotating buffer
	- This is one of the main memory issues right now
- Add Missing Textures
- Report to Windows
	- Stub Out Platform_win.cpp
	- Fix linking issues and install library's
	- Implement Platform_win.cpp
	- Fix Runtime Bugs
	- Commit back to upstream
	- Add console color support to windows
- Port to Linux
	- Fix runtime crash
	- Add linux release uploading to Travis
- Add new Scripting Features
	- Add require system
		- This will likely be based off require js or node's require system
	- Implement new functions in javascriptAPI.txt
		- Update and extend javascriptAPI.txt
	- Implement support for Packages in Javascript
		- There's a little right now but it should be full support
- Audio Support
	- Add OpenAL Depend (maybe another library, something to really look into)
	- Write C++ Classes
	- Write JS API (Maybe raw sound or maybe something more like web audio
- Support C based add-ons
	- Engine2D.h common API header file with wrapper code
		- Wrappers for important classes
			- Application
			- Window
			- Events
			- Draw2D
			- RenderDriver
		- In the future I could add C as a possible target for the script binding system
- Entity System
	- Abstract out messaging to work with entity's
	- Implement Serialization for the level format
- UI System
	- UI specific event systems
	- UI rendering framework
	- UI layout framework
	- UI Elements
		+ Button
		+ Listbox
		+ Textbox
		+ Checkbox
		+ Layout
		+ etc...
	- Loading UI from a source format
- Crypo/Security Features
	- Add OpenSSL
	- Add signed scripts/executable support
	- Add package encryption support
		- Add PackageEncryptionProvider
- Add Other scripting engine support
	- Remove all instances of v8::Isolate:GetCurrent
		- I heard that v8 will be removing this or deprecating it soon
	- Remove all instances of v8 outside of ScriptingManager
	- Add support for other Engines
		- LuaJit
		- Python
		- C# ??
			- With Microsoft's recent changes this could be done with their API
- Networking
	- The networking system will be tier based like the current rendering system
		- Enet
			- Get a feel for the libray
			- Write a small test in the engine with a very simple script binding
			- Get a feel for the performance and overhead
		- Events::NetworkTarget / EntityManager::NetworkSync ??
	- I'll ether roll my own sockets or use enet
- Multi-Threaded Rendering
	- Make rendering code thread safe/thread local
	- Multi-Thread JavaScript/Other scripting
- 3D Support
	- Lighting
		- Fully deferred rendering
		- Physically based rendering
	- Draw3D
