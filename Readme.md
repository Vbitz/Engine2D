# "Engine2D" 2D Game Engine

[![Build Status](https://travis-ci.org/Vbitz/Engine2D.svg?branch=master)](https://travis-ci.org/Vbitz/Engine2D)

## Auto-Generated Screenshot
![Screenshot](screenshot.png)

## What am I looking at?
Engine2D is a 2D Game Engine written in C++ with mature scripting support implemented using Chromium's V8 Javascript engine and graphics using OpenGL. Above is a screen shot of the interactive test suite testing Filesystem, Event dispatch among other core engine services.

## Building
Right now the engine builds out of the box on Mac OSX and Linux. To build it for other platforms you need...

- A modern C++ compiler supporting C++11. It's built using [clang](http://clang.llvm.org/) right now
- [Python](http://www.python.org/) v2.7.x (For GYP and tasks.py)
- [CMake](http://www.cmake.org/) (For GLFW)
- [GYP](https://code.google.com/p/gyp/)

On a source level you will need to fill in or stub out `Platform_{x}.cpp`.

Once you have the dependences installed you can run `./tasks.py build_bin` and you will hopefully find a executable has been built.

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

The latest version of the documentation generated using JSDoc can be found under doc/apiDocs or hosted at [http://vbitz.com/public/EngineAPIDocs/](http://vbitz.com/public/EngineAPIDocs/).

## Getting Started

- Copy the sample code and save it under res/script/helloWorld.js.
- Run `build/Default/engine2D -devmode -debug script/helloWorld` (You can substitute the path for any path under res/).
- If you want to edit your script then just change the file and save, the engine reloads automatically if `-devmode` is enabled.

## Contributing
Code Contributions are welcome large and small. Issues with scripting, running or building the engine can also be reported using the issue tracker to the right.

## License
Engine2D is licensed under the Apache 2 License. A copy can be found in LICENCE.
