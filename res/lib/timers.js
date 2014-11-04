/*
   Filename: timers.js
   Purpose:  Web JS Compatable Timers

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

(function (global) {
	function uuid() {
		return Math.floor(Math.random() * 1000000).toString(16);
	}

	global.setTimeout = function (cb, delay) {
		var eId = "timeout_" + uuid();
		sys.on(eId, eId, function () {
			sys.clearEvent(eId);
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