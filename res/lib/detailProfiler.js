/*
   Filename: detailProfiler.js
   Purpose:  JSON Profiler Frame Capture

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
	function start(filename, frames) {
		var currentFrames = 0,
			results = [],
			listenerName = "detailProfiler_listener_" + Math.random();

		global.sys.on("onProfileEnd", listenerName, function (args) {
			results.push(args);
			if (currentFrames++ >= frames) {
				global.sys.clearEvent(listenerName);
				console.toast("Saving profile results to: " + filename);
				global.fs.writeFile(filename, JSON.stringify(results));
				results = [];
				currentFrames = 0;
			}
		});
	}

	global.sys.on("detailProfile", "detailProfiler_start", function (args) {
		start(args.filename, args.frames);
	});

})(this);