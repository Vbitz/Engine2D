(function (global) {
	console._log("log", "Booting JavaScript Phase 1");

	function slowload(filename, persist, cb) {
		var loadUUID = Math.random().toString(16).substr(2);
		console._log("log", "slowload " + filename);
		global.sys.on("slowload_" + loadUUID, loadUUID, function (e) {
			global.sys.clearEvent(loadUUID);
			global.sys.runFile(filename, persist);
			cb();
		});
		global.sys.createTimer(1, "slowload_" + loadUUID);
	}

	console._log("log", "slowload: " + sys.config("core.debug.slowload"));

	var debugRunFile = sys.config("core.debug.slowload") ?
		slowload : function (filename, persist, cb) {global.sys.runFile(filename, persist); cb();};

	function makeLog(type) {
		return function () {
			var args = [type];
			for (var i = 0; i < arguments.length; i++) {
				args.push(arguments[i]);
			}
			console._log.apply(null, args);
		};
	}

	global.console.log = makeLog("log");
	global.console.warn = makeLog("warning");
	global.console.error = makeLog("error");
	global.console.verbose = makeLog("verbose");
	global.console.highlight = makeLog("highlight");
	global.console.toast = makeLog("toast");
	global.console.writeRaw = makeLog("raw");

	global.sys.drawFunc = function (func) {
		sys.on("draw", "sys.drawFunc", func);
	};

	global.sys.keyboardFunc = function (func) {
		sys.on("input", "sys.keyboardFunc", function (e) {
			func("", e.key, e.state === "press" || e.state === "repeat");
		});
	};

	var libraries = ["timers", "perlin", "detailProfiler"];
	var currentLib = 0;

	function runLibs(cb) {
		if (!libraries[currentLib]) {
			cb();
		} else {
			debugRunFile("lib/" + libraries[currentLib], false, function () {
				currentLib++;
				runLibs(cb);
			});
		}
	}

	runLibs(function () {
		var testNum = 0;

		global.sys.on("testingEventJS1", "boot.TestEventJS1", function () {
			if ((testNum % 50000) === 0) {
				console.log("[1] testNum = " + testNum);
			}
			testNum++;
		});

		global.sys.on("testingEventJS2", "boot.TestEventJS2", function () {
			if ((testNum % 50000) === 0) {
				console.log("[2] testNum = " + testNum);
			}
			testNum++;
		});

		global.sys.on("testScreenshot", "boot.testScreenshot", function () {
			if (!fs.hasSetConfigDir) {
				console.warn("configDir has not been set for TestScreenshot defaulting to testScreenshot");
				fs.configDir("testScreenshot");
			}
			global.sys.on("onSaveScreenshot", "boot.onSaveScreenshot", function (e) {
				console.log("TestScreenshot - #### Screenshot saved to [" + e.filename + "]");
			});
			console.log("TestScreenshot - Taking screenshot");
			event.screenshot({filename: "testScreenshot.bmp"});
			sys.exit();
		});

		function onPostLoad() {
			console.log("Booting JavaScript Phase 2");
			var argv = sys.argv();
			if (argv.length > 0) {
				sys.config("core.script.entryPoint", argv[0]);
			}
			var versionInfo = sys.version();
			console.log("Using OpenGL: " + versionInfo.openGL);
			console.log("Using OpenGL Renderer: " + versionInfo.glRenderer);
			console.log("Using entryPoint: " + sys.config("core.script.entryPoint"));
			sys.screenWidth = parseInt(sys.config("core.window.width"), 10);
			sys.screenHeight = parseInt(sys.config("core.window.height"), 10);
			sys.clearEvent("bootloaderLoad");
			if (!sys.runFile(sys.config("core.script.entryPoint"), true)) {
				console.error("Could not load core.script.entryPoint=" + sys.config("core.script.entryPoint"));
				console.toggle();
			}
			if (sys.config("core.test.screenshotTime") > 0) {
				sys.createTimer(sys.config("core.test.screenshotTime"), "testScreenshot");
			}
		}

		if (sys.preload) {
			sys.on("postLoad", "boot.bootloaderLoad", onPostLoad);
		} else {
			if (sys.config("core.debug.slowload")) {
				onPostLoad();
			} else {
				console.log("Rebooting");
				sys.restartRenderer();
				onPostLoad();
			}
		}
	});
})(this);