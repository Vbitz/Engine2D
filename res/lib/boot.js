// TODO: Make a better loader

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

var str = fs.readFile("config/config.json");

console.log("Booting JavaScript Phase 1");

console.log("Current Config: " + str);

var globalConfig = {};

try {
	globalConfig = JSON.parse(str);
} catch (e) {
	console.log("Error Loading config: " + e);
}

function getConfig(str, def) {
	if (globalConfig[str]) {
		return globalConfig[str];
	} else {
		return def;
	}
}

sys.setWindowCreateParams(getConfig("window", {}));

var libarys = ["ui"];

for (var i = 0; i < libarys.length; i++) {
	sys.runFile("lib/" + libarys[i], false);
}

sys.onPostLoad(function () {
	console.log("Booting JavaScript Phase 2");
	var argv = sys.argv();
	var glInfo = sys.getGLVersion();
	console.log("Using OpenGL: " + glInfo.major + "." + glInfo.minor + "." + glInfo.rev);
	sys.runFile(argv.length > 1 ? argv[1] : getConfig("startup", "script/basic"), true);
});