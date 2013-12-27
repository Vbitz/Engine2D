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

sys.drawFunc = function (func) {
	sys.clearEvent("sys.drawFunc");
	sys.on("draw", "sys.drawFunc", func);
}

sys.keyboardFunc = function (func) {
	sys.clearEvent("sys.keyboardFunc");
	sys.on("input", "sys.keyboardFunc", function (e) {
		func("", e["key"], e["state"] === "press" || e["state"] === "repeat");
	});
}

var str = fs.readFile("config/config.json");

console.log("Booting JavaScript Phase 1");

var globalConfig = {};

try {
	globalConfig = JSON.parse(str);
} catch (e) {
	console.log("Error Loading config: " + e);
}

sys.setWindowCreateParams(globalConfig);

var libarys = ["ui"];

for (var i = 0; i < libarys.length; i++) {
	sys.runFile("lib/" + libarys[i], false);
}

function onPostLoad() {
	console.log("Booting JavaScript Phase 2");
	var argv = sys.argv();
	var glInfo = sys.getGLVersion();
	console.log("Using OpenGL: " + glInfo.major + "." + glInfo.minor + "." + glInfo.rev);
	sys.runFile(argv.length > 0 ? argv[0] : sys.config("core.script.entryPoint"), true);
	sys.clearEvent("bootloaderLoad");
}

if (sys.preload) {
	sys.on("postLoad", "bootloaderLoad", onPostLoad);
} else {
	console.log("Rebooting");
	sys.restartRenderer();
	onPostLoad();
}