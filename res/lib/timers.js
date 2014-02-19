(function (global) {
	function uuid() {
		return Math.floor(Math.random() * 1000000).toString(16);
	}

	global.setTimeout = function (cb, delay) {
		var eId = "timeout_" + uuid();
		sys.on(eId, eId, function () {
			cb();
		});
		return sys.createTimer(Math.floor(delay / 1000), eId);
	};

	global.clearTimeout = function (id) {
		sys.deleteTimer(id);
	};

	global.setInterval = function (cb, delay) {
		var eId = "interval_" + uuid();
		sys.on(eId, eId, function () {
			cb();
		});
		return sys.createTimer(Math.floor(delay / 1000), eId, true);
	};

	global.clearInterval = function (cb) {
		sys.deleteTimer(id);
	};
})(this);