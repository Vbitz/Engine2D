(function (global) {
	var listenerName = "detailProfiler_listener";

	function start(filename, frames) {
		var currentFrames = 0,
			results = [];

		global.sys.on("onProfileEnd", listenerName, function (args) {
			results.push(args);
			if (currentFrames++ >= frames) {
				global.sys.clearEvent(listenerName);
				console.toast("Saving profile results to: " + filename);
				global.fs.writeFile(filename, JSON.stringify(results));
				results = [];
			}
		});
	}

	global.sys.on("detailProfile", "detailProfiler_start", function (args) {
		start(args.filename, args.frames);
	});

})(this);