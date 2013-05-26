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

//sys.runFile("script/basic", true);
sys.runFile("lib/ui", true);
//sys.runFile("script/uiTest", true);
//sys.runFile("script/startupNew", true);
//sys.runFile("script/startupCustomImage", true);
//sys.runFile("script/gol", true);
sys.runFile(getConfig("startup", "script/basic"), true);