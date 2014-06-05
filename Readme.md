# "Engine2D" 2D Game Engine

## Auto-Generated Screenshot
![Screenshot](screenshot.png)

## Building
Right now the engine **only** builds out of the box on Mac OSX. To build it for other platforms you need...

- A modern C++ compiler supporting C++11. It's built using [clang](http://clang.llvm.org/) right now
- [Python](http://www.python.org/) v2.7.x (For GYP and tasks.py)
- [Subversion](https://subversion.apache.org/) (For GYP)
- [GYP](https://code.google.com/p/gyp/)
- [FreeType](http://www.freetype.org/) v2.5.x
- [FreeImage](http://freeimage.sourceforge.net/) v3.15.x
- [GLFW](http://www.glfw.org/) v3.0.x
- [PhysFS](https://icculus.org/physfs/) v2.0.x
- [V8](https://code.google.com/p/v8/) v3.25.30
- [GLM](http://glm.g-truc.net/0.9.5/index.html) v0.9.x

On a source level you will need to fill in or stub out `Platform_{x}.cpp`.

Once you have the dependancys installed you can run `./tasks.py gyp` and then build the project using XCode on OSX, Visual Studio on Windows or make on linux.

If your having trouble locating or building the dependancys then there are incomplete instructions in tasks.py. You can also fetch almost all of the dependancys using Homebrew with `brew install physfs v8 freeimage glm freetype`.

##Sample Code

```javascript
sys.on("draw", "helloWorld.draw", function (e) {
	draw.setColor("blue");
	draw.rect(100, 100, 100, 100);
});
```

##Sample Code 2

```javascript
/* jshint esnext: true */

var counter = 0;
var circleColor = 0x101010;

function* genGradient() {
	while (true) {
		for (var x = 0; x < 360; x++) {
			yield draw.getRGBFromHSV(x, 1.0, 0.5);
		}
	}
}

var generator = genGradient();

setInterval(function () {
	circleColor = generator.next().value;
}, 100);

sys.on("draw", "helloWorld2.draw", function () {
	draw.setColor(circleColor);
	draw.rect(Math.sin(counter) * 100 + 150,
		Math.sin(counter) * 100 + 150, 100, 100);
	counter += sys.deltaTime + (Math.random() / 16);
});
```

##Documentation

The latest version of the documentaion generated using JSDoc can be found under doc/apiDocs or hosted at [http://vbitz.com/public/EngineAPIDocs/](http://vbitz.com/public/EngineAPIDocs/).

## Getting Started

- Copy the sample code and save it under res/script/helloWorld.js.
- Run `build/Default/engine2D -devmode -debug script/helloWorld` (You can substiute the path for any path under res/).
- If you want to edit your script then just change the file and save, the engine reloads automaticly if `-devmode` is enabled.

## Contributing
Code Contibutions are welcome. If you don't like coding then open a issue.

## Licence
Engine2D is licenced under the Apache 2 Licence. A copy can be found in LICENCE.
