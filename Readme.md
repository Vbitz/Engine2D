OSX Build Guide
-------------------
- Install xcode or xcode Command Line Tools
- Install Homebrew
- Clone the project to a directory of your choice
- Install `glew physfs v8 freeimage glm freetype` using Homebrew
- Clone and install https://github.com/glfw/glfw
- Run `make run` in the project directory
- If you want a .app then Run `make release`. The bundle will then be generated to release-osx/

Windows Build Guide
-------------------
Windows binarys are avalible from: http://vbitz.com/public/engine2d-bins-win32-01.zip
This is not for the fait of heart. In the future I will provide project files or some kind of binary dependancys.
- Install Visual Studio (Express 2012 for Windows Desktop) is what I tested the compile on)
- Install CMake (For GLFW and PhysFS)
- Install Git for Windows (Or any github client)
- Clone the project into an empty directory, any directory works
- Make a new directory to store collected libs and headers
- Create a new C++ Console Application Project and add all the files in src and src/extern
- Download and extract v8 from https://github.com/v8/v8/archive/3.19.18.4.tar.gz (The latest version breaks the way I create methods in C++)
- Compile V8 in Release mode, this is complex so I will just link their guide https://code.google.com/p/v8/wiki/BuildingWithGYP
- Copy the .lib files from the output folder to your collected libary folder
- Copy the contents of the include folder to your collected include folder
- Clone https://github.com/glfw/glfw and generate VS project files
- You need to change the Code Generation setting in visual stuido from `/MD` to `/MT`
- Now you can build glfw in release mode
- Copy the .lib files from the output folder to your collected libary folder
- Copy the contents of the include folder to your collected include folder
- Do the same with PhysFS
- FreeImage, Glew and FreeType all come in binary forms so just download those and copy the DLL's and other files into the collected folders
- GLM is header only so just download it and copy the files into your collected Include directory
- SQLite is distributed as source files so just download sqlite 2.0.3 and copy the files into your project
- That should be all the dependancys sorted so now you need to configure the VS project
- Engine2D only compiles in release mode right now so switch it now
- You need to change the Code Generation Setting in visual stuido for Engine2D from `/MD` to `/MT`
- Under Linker\Input in the project properties you need to add `MSVCRT.lib` to Ignore Specifc Default Libarys
- Under Additional Dependancys in the same window you need to link `OpenGL32.lib;Freeimage.lib;freetype.lib;glew32.lib;glfw3.lib;physfs.lib;v8_snapshot.lib;v8_base.ia32.lib;wsock32.lib;winmm.lib`. All of these other then OpenGL, wsock and winmm should be in your collected project folder
- Make sure SubSystem is set as console under Linker\System in the project properties
- Now you should be able to complie the project. When it goes to run it will fail with a dialog, don't worry we just need to copy the DLL's and resources
- Copy the contents of res\ into your solution Release folder
- Copy `freetype6.dll`, `glew32.dll` and `FreeImage.dll` into the same folder
- Finaly set the Working Directroy under Debugging to `$(TargetDir)` so it finds all the resources
- It should compile and run correctly, if it does not then post the Error as a new Issue

Linux Build Guide
-------------------
I have not tested a Linux version yet but you should be able to mostly follow the OSX instructions

Sample Code
-------------------
```javascript
sys.drawFunc(function () {
	draw.setColor("blue");
	draw.rect(100, 100, 100, 100);
});
```

Getting Started
-------------------
Right now the engine is in beta but if you want to try it out then you can start writing JS code by
- Creating a JS file with the sample code
- Editing the startup key in res/config/config.json with your own script filename
- Run the engine with `make run`
- If you want to edit your script then just change the file and save, the engine reloads automaticly by default