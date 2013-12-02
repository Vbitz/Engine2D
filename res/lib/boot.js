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
	sys.on("draw", "sys.drawFunc", func);
}

sys.keyboardFunc = function (func) {
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

var postLoadID = -1;

function onPostLoad() {
	console.log("Booting JavaScript Phase 2");
	var argv = sys.argv();
	var glInfo = sys.getGLVersion();
	console.log("Using OpenGL: " + glInfo.major + "." + glInfo.minor + "." + glInfo.rev);
	sys.runFile(argv.length > 0 ? argv[0] : sys.config("js_startupScript"), true);
	if (postLoadID != -1) { // make sure this does not get called twice
		sys.clearEvent(postLoadID);
		postLoadID = -1;
	}
}

if (sys.preload) {
	postLoadID = sys.on("postLoad", "bootloaderLoad", onPostLoad);
} else {
	console.log("Rebooting");
	sys.restartRenderer();
	onPostLoad();
}