Engine2D
========

OSX Build Guide
-------------------
- Install xcode or xcode Command Line Tools
- Install Homebrew
- Clone the project to a directory of your choice
- Install ```glew physfs v8 freeimage glm freetype``` using Homebrew
- Clone and install https://github.com/glfw/glfw
- Run ```make run``` in the project directory

Sample Code
-------------------
javascript```
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
- Run the engine with ```make run```