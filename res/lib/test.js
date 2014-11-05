/* jshint esnext: true */

/*
   Filename: test.js
   Purpose:  Javascript Test Suite

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

/***
	{
		"fontCacheList": [
			"fonts/open_sans.json"
		],
		"textureCacheList": [
			"texture/spriteTest.json",
			"texture/testing.png"
		]
	}
*/

fs.configDir("testSuite");

console.log("Loading Test Suite");

var tests = {
	"Filesystem": function () {
		fs.writeFile("testing.txt", "Hello world");
		return fs.readFile("testing.txt") === "Hello world";
	},
	"Database": function () {
		var testDb = new db.Database("test.db"),
			testDb2 = new db.Database("test2.db");

		testDb.exec("DROP TABLE IF EXISTS testing");
		testDb.exec("CREATE TABLE testing(t1 TEXT)");
		testDb.exec("INSERT INTO testing VALUES('hello world')");

		testDb2.exec("DROP TABLE IF EXISTS testing2");
		testDb2.exec("CREATE TABLE testing2(t1 TEXT)");
		testDb2.exec("INSERT INTO testing2 VALUES('" + testDb.execPrepare("SELECT * FROM testing")[0].t1 + "')");

		return testDb2.execPrepare("SELECT * FROM testing2")[0].t1 === "hello world";
	},
	"Unsafe": function () {
		if (!sys.runFile("modules/js_unsafe.dylib", false)) {
			return false; // If we can't load the module then fail the test
		}

		// get the pagesize
		var pagesize = unsafe.getPageSize();

		function copy(a, b, bOff, len) {
			for (var i = 0; i < len; i++) {
				b[i + bOff] = a[i];
			}
		}

		// disassembled:
		//		ret
		var instructions = [0xc3];

		// add some more space to the buffer for page alignment
		var bufSize = 32 + pagesize;

		// malloc the buffer
		var buf = unsafe.malloc(bufSize);

		// get the real address to the buffer to calucate the offset to write the machine code to
		var realBufAddress = unsafe.addressOf(buf);
		var bufAddr = unsafe.addressOf(buf);

		// align bufAddr to the closest memory page (otherwise mprotect won't work and calling will crash node)
		bufAddr -= (bufAddr % pagesize);
		bufAddr += pagesize;

		// copy the instructions to the buffer aligned to page boundarys
		copy(instructions, buf, (bufAddr - realBufAddress), instructions.length);

		// call mprotect() to make sure we can run the code
		if (!unsafe.mprotect(bufAddr, bufSize, true)) {
			throw new Error("mprotect failed");
		} else {
			// call the code
			unsafe.call(bufAddr);

			// disable execution and reenable write access so free() stays safe
			unsafe.mprotect(bufAddr, bufSize, false);
		}

		// finaly free the buffer
		unsafe.free(bufAddr);

		return true;
	},
	"Harmony": function () {
		"use strict";

		function* testGenerator() {
			yield "hello";
			yield "world";
			yield "this";
			yield "is";
			yield "a";
			yield "test";
		}

		var items = ["hello", "world", "this", "is", "a", "test"];

		var iter = testGenerator();

		for (var i = 0; i < items.length; i++) {
			var iterValue = iter.next().value;
			if (iterValue !== items[i]) {
				console.error("failed: ", iterValue, items[i]);
				return false;
			}
		}

		return true;
	},
	"Threading": function () {
		sys.on("testThreadTarget", "test.testThreadTarget", function (e) {
			console.log("From TestThread: " + e.value);
		});

		sys.createWorker(function (thread) {
			var t = 0;
			for (var i = 0; i < 5; i++) {
				thread.log("t = " + t);
				t++;
				thread.sleep(0.2);
			}
			thread.log(t);
			thread.emit("testThreadTarget", {value: Math.random()});
		});

		return true;
	},
	"EventMagic": function () {
		var count = 0;
		sys.on("eventMagicTestTarget", "test.eventMagicTestTarget1", function (e) {
			count++;
			return event.EM_CANCEL;
		});
		sys.on("eventMagicTestTarget", "test.eventMagicTestTarget2", function (e) {
			count++;
		});
		sys.emit("eventMagicTestTarget");
		return count === 1;
	},
	"DeferedEvents": function () {
		var valid = false;
		sys.on("deferedEventsTestRouter", "test.DeferedEventsTestRouter", function (e) {
			sys.pollDeferedMessages("deferedEventsTestTarget");
		});
		sys.on("deferedEventsTestTarget", "test.DeferedEventsTestTarget", function (e) {
			valid = true;
		});
		sys.setDeferedEvent("deferedEventsTestTarget", true);
		sys.emit("deferedEventsTestTarget");
		sys.emit("deferedEventsTestRouter");
		return valid;
	},
	"Random": function () {
		var rand = new Math.Random(1337);

		if (rand.next() !== 1125387415) return false;
		if (rand.nextDouble() !== 0.15868397722033079) return false;

		return true;
	},
	"Vector": function () {
		var v = new Math.Vector(1, 2, 3);
		
		var result = v.add(new Math.Vector(1, 2, 3));

		var v2 = new Math.Vector(1, 2, 3, 4);

		v2 = v2.add(new Math.Vector(1, 2, 3, 4));
		v2 = v2.sub(Math.Vector(0, 2, 1));
		v2 = v2.mul(Math.Vector(1, 2, 3));
		v2 = v2.cross(new Math.Vector(1, 3, 10, 4));
		var v3 = v2.dot(new Math.Vector(1, 2, 3, 4));

		return result.x == 2 && v3 == -9;
	},
	"Matrix": function () {
		var m1 = new Math.Matrix();

		m1.translate(new Math.Vector(0, 5, 0));

		m1.scale(new Math.Vector(0, 5, 0));
		
		m1.rotate(Math.degToRad(45), new Math.Vector(0, 5, 0));

		var m2 = Math.Matrix.createLookAt(
			new Math.Vector(5, 5, 5),
			new Math.Vector(10, 10, 10));
			
		return true;
	},
	"Package": function () {
		var pkg = new sys.Package("testing_output.epkg");

		if (!pkg.fileExists("test.js")) {
			return false;
		}

		var str = pkg.readFile("test.js");

		console.log("res://testing_output.epkg/test.js length=" + str.length);

		return str.length > 0;
	}
};

var testResults = {};
var passedTests = 0;
var failedTests = 0;

for (var i in tests) {
	try {
		console.log("Running JS Test : " + i);
		testResults[i] = tests[i]();
		if (testResults[i]) {
			console.log("Test passed : " + i);
			passedTests++;
		} else {
			console.error("Test failed : " + i);
			failedTests++;
		}
	} catch (e) {
		console.error("Test failed : " + i + " : " + e.message);
		failedTests++;
		testResults[i] = false;
	}
}

console._log("highlight", "test.js Tests Complete: " + passedTests + " passed : " + failedTests + " failed");

draw.clearColor(0x010101);

var pix = new Float32Array(100 * 100 * 4);

for (var i = 0; i < 100 * 100 * 4; i += 4) {
	pix[i] = Math.cos(i);
	pix[i + 1] = Math.sin(i + 1);
	pix[i + 2] = Math.tan(i + 2);
	pix[i + 3] = 1.0;
}

var hsv = new Float32Array(360 * 100 * 4);

var point = 0;

for (var x = 0; x < 360; x++) {
	for (var y = 0; y < 100; y++) {
		var rgb = draw.getRGBFromHSV(x, 1.0, y / 100);
		hsv[point++] = rgb.r;
		hsv[point++] = rgb.g;
		hsv[point++] = rgb.b;
		hsv[point++] = 1.0;
	}
}

var img = img ? img : new draw.Texture("texture/testing.png");
var img2 = img2 ? img2 : new draw.Texture(pix, 100, 100);
var img3 = img3 ? img3 : new draw.Texture(hsv, 360, 100);
//var mImgA = mImgA ? mImgA : draw.getImageArray("texture/testing.png");

//for (var i = 0; i < mImgA.length; i += 4) {
//	mImgA[i + 0] = 1 - mImgA[i + 0];
//	mImgA[i + 1] = 1 - mImgA[i + 1];
//	mImgA[i + 2] = 1 - mImgA[i + 2];
//}

//var mImg = mImg ? mImg : draw.createImage(mImgA, 128, 128);

var spriteSheet = spriteSheet ? spriteSheet : (function () {
	console.log("Loading Sprite Sheet");
	return draw.openSpriteSheet("texture/spriteTest.json");
})();

var t = 0;

var supportsFramebuffer = sys.hasExtention("GL_ARB_framebuffer_object")|| sys.hasExtention("GL_EXT_framebuffer_object");

console.log("OpenGL Extentions:");
sys.getExtentions().forEach(function (i) {
	console.log("    " + i);
});

var firstDraw = true;

sys.on("testTimer1", "test.timer", function () {
	console.log("[createTimer interval] This runs every 5 seconds");
});

sys.on("testTimer2", "test.timer", function () {
	console.log("[createTimer] This runs once after 10 seconds");
});

sys.createTimer(5, "testTimer1", true);
sys.createTimer(10, "testTimer2");

setInterval(function () {
	console.log("[setInterval] This runs every 5 seconds");
}, 5000);

setTimeout(function () {
	console.log("[setTimeout] This runs once after 10 seconds");
}, 10000);


var vBuff = new draw.VertexBuffer2D("shaders/basic.json");

vBuff.setDepthTest(true);
vBuff.setProjectionPerspective(true);
vBuff.setLookAtView(2.0, 2.0, 2.0, 0, 0, 0);

// add a triangle
var vCol = new draw.Color(0x303030);
vBuff.addVert(-1, -1, vCol);
vBuff.addVert(1, -1, vCol);
vBuff.addVert(1, 1, vCol);
vBuff.addVert(-1, -1, vCol);
vBuff.addVert(-1, 1, vCol);
vBuff.addVert(1, 1, vCol);

var graphicsTests = {
	
};

sys.on("draw", "test.draw", function (e) {
	if (!draw.isSpriteSheet(spriteSheet)) {
		console.log("Loading Sprite Sheet");
		spriteSheet = draw.openSpriteSheet("texture/spriteTest.json");
	}

	if (!draw.isFontLoaded("light")) {
		console.log("Loading Font");
		draw.loadFont("light", "fonts/open_sans.json");
	}

	if (firstDraw) {
		console.log("Finished Loading");
		firstDraw = false;
	}

	vBuff.model.rotate(sys.deltaTime, new Math.Vector(0, 1, 0));
	vBuff.draw();

	draw.grad(0, 25, 600, 35, 0xfafafa, 0x000000, false);
	draw.drawColor = "black";
	draw.setFont("basic", 16);
	draw.print(10, 35, sys.version().engine + " Test Suite");

	draw.drawColor = "white";
	draw.print(10, 70, "Automated Tests");
	draw.print(400, 70, "Rendering Tests");

	draw.setFont("basic", 12);
	var x = 2;
	for (var i in testResults) {
		if (testResults[i]) {
			draw.drawColor = "green";
		} else {
			draw.drawColor = "red";
		}
		draw.rect(15, x * 30 + 35, 220, 20);
		draw.drawColor = "black";
		draw.print(20, x * 30 + 40, i);
		x++;
	}

	draw.drawColor = 0xffffff;
	draw.print(10, x++ * 30 + 40, "Platform: " + sys.platform);
	draw.print(10, x++ * 30 + 40, "Supports Framebuffer: " + supportsFramebuffer);
	draw.print(10, x++ * 30 + 40, "Number of Processers: " + sys.numProcessers);
	if (sys.config("core.test.screenshotTime") === 0) {
		draw.print(10, x++ * 30 + 40, "Username: " + sys.username);
	}

	draw.setFont("light", 8);
	draw.print(20, x * 30 + 40, "Custom Loaded Font");

	draw.setFont("basic", 12);
	draw.print(420, 100, "Rect");
	draw.rect(410, 120, 100, 100);

	draw.print(540, 100, "Grid");
	draw.grid(530, 120, 100, 100);

	draw.print(650, 100, "Gradents");
	draw.grad(650, 120, 100, 100, 0xffffff, 0x000000, true);
	draw.grad(770, 120, 100, 100, 0xffffff, 0x000000, false);

	draw.print(400, 240, "PNG Image");
	draw.drawColor = "white";
	draw.draw(img, 410, 270, 100, 100);

	draw.print(520, 240, "Gen Image");
	draw.drawSub(img2, 530, 270, 100, 100, t, t += 0.1, 16, 16);

	draw.print(640, 240, "Tinted Image");
	draw.drawColor = draw.Color.fromRandom();
	draw.draw(img, 650, 270, 100, 100);

	draw.drawColor = "white";
	draw.print(760, 240, "Circle");
	draw.circle(770 + 50, 270 + 50, 50, 40, 200, 0.1, 1.0, true);

	//draw.print(900, 240, "Modded Image");
	//draw.draw(mImg, 900, 270, 100, 100);

	draw.print(400, 380, "Sprite Sheet");
	draw.drawSprite(spriteSheet, "idle", 410, 390, 100, 100);

	draw.print(400, 500, "HSV Image");
	draw.draw(img3, 410, 520, 360, 100);

// profiler
/*
	draw.setFont("basic", 16);
	draw.print(1040, 70, "Profiler");

	draw.setFont("basic", 12);
	var y = 100;
	var zones = sys.getProfilerZones();
	var h = zones.length * 20;
	draw.setColor(0xcccccc);
	draw.rect(1030, y - 10, 150, h + 10);
	zones.forEach(function (i) {
		draw.setColor(0x010101);
		draw.print(1050, y, i);
		draw.setColor(0xfafafa);
		draw.print(1190, y, sys.getProfilerTime(i).toFixed(4));
		y += 20;
	});
*/
	if (sys.config("core.test.screenshotTime") > 0) {
		for (var curveX = 0; curveX < 2000; curveX += 100) {
			draw.curve(curveX, 500, curveX, 600, 850, 600, curveX, 500);
		}
	} else {
		for (var curveX = 0; curveX < 2000; curveX += 100) {
			draw.curve(curveX, 500, curveX, 600, 850, 600, input.mouseX, input.mouseY);
		}	
	}
});

event.key_S = function test_screenshot(e) {
	if (e.state === "press") {
		event.screenshot({filename: "testing.png"});
	}
};

event.key_F = function test_fullscreen(e) {
	if (e.state === "press") {
		event.toggleFullscreen();
	}
};

event.key_P = function test_profile(e) {
	if (e.state === "press") {
		console.log("Profiling for 50 frames");
		event.detailProfile({filename: "profile.log", frames: 50});
	}
}

sys.on("input", "test.input", function (e) {
	var key = e.key;
	var press = e.state === "press" || e.state === "repeat";

	console.log("[INPUT] : (" + key.charCodeAt(0) + ") " + key + " : " + press);
});

sys.on("mouseButton", "test.mouseButton", function (e) {
	console.log("[MOUSE] : " + e.button + " : " + e.action + " : (" + e.x + ", " + e.y + ")");
});