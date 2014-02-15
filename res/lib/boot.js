console._log("log", "Booting JavaScript Phase 1");

function makeLog(type) {
	return function () {
		var args = [type];
		for (var i = 0; i < arguments.length; i++) {
			args.push(arguments[i]);
		}
		console._log.apply(null, args);
	};
}

console.log = makeLog("log");
console.warn = makeLog("warning");
console.error = makeLog("error");
console.verbose = makeLog("verbose");
console.writeRaw = makeLog("raw");

sys.drawFunc = function (func) {
	sys.clearEvent("sys.drawFunc");
	sys.on("draw", "sys.drawFunc", func);
};

sys.keyboardFunc = function (func) {
	sys.clearEvent("sys.keyboardFunc");
	sys.on("input", "sys.keyboardFunc", function (e) {
		func("", e.key, e.state === "press" || e.state === "repeat");
	});
};

var libarys = ["ui", "timers", "perlin"];

libarys.forEach(function (lib) {
	sys.runFile("lib/" + lib, false);
});

function onPostLoad() {
	console.log("Booting JavaScript Phase 2");
	var argv = sys.argv();
	if (argv.length > 0) {
		sys.config("core.script.entryPoint", argv[0]);
	}
	var glInfo = sys.getGLVersion();
	console.log("Using OpenGL: " + glInfo.major + "." + glInfo.minor + "." + glInfo.rev);
	sys.screenWidth = parseInt(sys.config("core.window.width"), 10);
	sys.screenHeight = parseInt(sys.config("core.window.height"), 10);
	sys.clearEvent("bootloaderLoad");
	if (!sys.runFile(sys.config("core.script.entryPoint"), true)) {
		console.error("Could not load core.script.entryPoint=" + sys.config("core.script.entryPoint"));
		console.toggle();
	}
}

if (sys.preload) {
	sys.on("postLoad", "bootloaderLoad", onPostLoad);
} else {
	console.log("Rebooting");
	sys.restartRenderer();
	onPostLoad();
}