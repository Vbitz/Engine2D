// TODO: Make a better loader

var str = fs.readFile("config/config.json");

console.log(str);

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
	sys.runFile(getConfig("startup", "script/basic"), true);
});