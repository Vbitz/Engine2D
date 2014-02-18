OSX Build Guide
-------------------
- Install xcode
- Install Homebrew
- Clone the project to a directory of your choice
- Install `glew physfs v8 freeimage glm freetype` using Homebrew
- Clone and install https://github.com/glfw/glfw (You can also get a Homebrew formula from https://github.com/Homebrew/homebrew-versions/blob/master/glfw3.rb)
- `svn checkout http://gyp.googlecode.com/svn/trunk third_party/gyp`
- `./tasks.py build_bin`

If you want to build without Homebrew I've started to put together a automated build system inside of tasks.py

Linux Build Guide
-------------------
I have not tested a Linux version yet but you should be able to mostly follow the OSX instructions

Sample Code
-------------------
```javascript
sys.on("draw", "helloWorld", function (e) {
	draw.setColor("blue");
	draw.rect(100, 100, 100, 100);
});
```

Documentation
-------------------
The latest version of the documentaion generated using JSDoc can be found under doc/apiDocs or hosted at http://vbitz.com/public/EngineAPIDocs/

Getting Started
-------------------
Right now the engine is in beta but if you want to try it out then you can start writing JS code by
- Creating a JS file with the sample code and save it under res/script/helloWorld.js
- Run `build/Default/engine2D -devmode -debug script/helloWorld` (You can substiute the path for any path under res/)
- If you want to edit your script then just change the file and save, the engine reloads automaticly by default