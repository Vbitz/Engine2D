# Engine2D Package System "Vision"

Firstly I'm calling this a **vision** because while it does dictate where I want the Package/Require system to go some of these points are very very far off. Take this all with a grain of salt OK. I'm only writing this here because it's too complex for Trello (Where I have most of my goals and to-do's for the engine).

### Why write a package system?
Right now the engine has `sys.loadFile(filename, persist)`. As developers of JavaScript and other languages far and wide have found out over the past few years to scale the development of large applications you need to organise your code effectively. NodeJS has `var mod = require("mod")`, Ruby has RubyGems along with the language features that support that.

Package systems give support for some really complex behaviour like...

* Use Web URL's as namespaces (Look at Google Go for a example)
* Manage package versioning.
* Split away optional functionality as external modules.
* Load packages async. (Node does this to a extent, AMD is based around this concept)
* Code libarys.
* Easier to package code for distribution.

### Why exactly is this useful for a Game Engine?
As I start developing more games (however toy scale they may be) I keep seeing common code, code that would lead to too much bloat if I included it by default.

### What would this look like?
The basic example for the engine taken from the Github read me right now is...

```
	sys.on("draw", "helloWorld", function () {
		draw.setColor("blue");
		draw.rect(100, 100, 100, 100);
	});
```

With the new system this would change to...

```
	define(["draw"], function (draw) {
		$("draw", function () {
			draw.setColor("blue");
			draw.rect(100, 100, 100, 100);			}			});
```

The new system implaments AMD loading in C++, this has advantages now and towards the future. It will make streaming assets easier and allow for built in versioning.