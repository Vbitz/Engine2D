# "Engine2D" 2D Game Engine

## Building
Right now the engine **only** builds on Mac OSX. To build it for other platforms you need...

- A modern C++ compiler supporting C++11. It's built using [clang](http://clang.llvm.org/) right now
- [Python](http://www.python.org/) v2.7 (For GYP and tasks.py)
- [Subversion](https://subversion.apache.org/) (For GYP)
- [GYP](https://code.google.com/p/gyp/)
- [FreeType](http://www.freetype.org/) v2.5.x
- [FreeImage](http://freeimage.sourceforge.net/) v3.15.x
- [GLEW](http://glew.sourceforge.net/) v1.1.x
- [GLFW](http://www.glfw.org/) v3.0.x
- [PhysFS](https://icculus.org/physfs/) v2.0.x
- [V8](https://code.google.com/p/v8/) v3.21.x

On a source level you will need to fill in or stub out Platform_x.cpp.

Once you have the dependancys installed you can run `./tasks.py gyp` and then build the project using XCode on OSX, Visual Studio on Windows or make on linux.

If your having trouble locating or building the dependancys then there are incomplete instructions in tasks.py. You can also fetch almost all of the dependancys using Homebrew with `brew install glew physfs v8 freeimage glm freetype`.##Sample Code

```javascript
sys.on("draw", "helloWorld.draw", function (e) {
        draw.setColor("blue");
        draw.rect(100, 100, 100, 100);
});
```

##Documentation

The latest version of the documentaion generated using JSDoc can be found under doc/apiDocs or hosted at [http://vbitz.com/public/EngineAPIDocs/](http://vbitz.com/public/EngineAPIDocs/).

## Getting Started

- Copy the sample code and save it under res/script/helloWorld.js.
- Run `build/Default/engine2D -devmode -debug script/helloWorld` (You can substiute the path for any path under res/).
- If you want to edit your script then just change the file and save, the engine reloads automaticly if `-devmode` is enabled.

## Contributing
Code Contibutions are welcome. If you don't like coding then please open a issue.

## Licence
Engine2D is licenced under the Apache 2 Licence. A copy can be found in LICENCE