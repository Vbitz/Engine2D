/* jshint esnext: true */
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
		db.open("test.db");

		db.exec("DROP TABLE IF EXISTS testing");
		db.exec("CREATE TABLE testing(t1 TEXT)");
		db.exec("INSERT INTO testing VALUES('hello world')");

		return db.execPrepare("SELECT * FROM testing")[0].t1 === "hello world";
	},
	"Module": function () {
		var m = mod.open("modules/testing");

		mod.call(m, "testing");

		return true;
	},
	"Unsafe": function () {
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
			let iterValue = iter.next().value;
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
	}
};

var testResults = {};
var passedTests = 0;
var failedTests = 0;

for (var i in tests) {
	try {
		console.log("Running JS Test : " + i);
		sys.perf(i + "_test", function () {
			testResults[i] = tests[i]();
		});
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

var pix = draw.createImageArray(100, 100);

for (var i = 0; i < 100 * 100 * 4; i += 4) {
	pix[i] = Math.cos(i);
	pix[i + 1] = Math.sin(i + 1);
	pix[i + 2] = Math.tan(i + 2);
}

var hsv = draw.createImageArray(360, 100);

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

var img = img ? img : draw.openImage("texture/testing.png");
var img2 = img2 ? img2 : draw.createImage(pix, 100, 100);
var img3 = img3 ? img3 : draw.createImage(hsv, 360, 100);
var mImgA = mImgA ? mImgA : draw.getImageArray("texture/testing.png");

for (var i = 0; i < mImgA.length; i += 4) {
	mImgA[i + 0] = 1 - mImgA[i + 0];
	mImgA[i + 1] = 1 - mImgA[i + 1];
	mImgA[i + 2] = 1 - mImgA[i + 2];
}

var mImg = mImg ? mImg : draw.createImage(mImgA, 128, 128);

var spriteSheet = spriteSheet ? spriteSheet : (function () {
	console.log("Loading Sprite Sheet");
	return draw.openSpriteSheet("texture/spriteTest.json");
})();

var t = 0;

var supportsFramebuffer = sys.hasExtention("GL_ARB_framebuffer_object") || sys.hasExtention("GL_EXT_framebuffer_object");

console.log("Loading Test Suite");

/*
console.log("Extentions");
sys.getExtentions().forEach(function (i) {
	console.log(i);
});
*/

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


var graphicsTests = {
	
};

sys.on("draw", "test.draw", function (e) {
	if (!draw.isTexture(img)) {
		console.log("Loading Image 1");
		img = draw.openImage("texture/testing.png");
	}

	if (!draw.isTexture(img2)) {
		console.log("Loading Image 2");
		img2 = draw.createImage(pix, 100, 100);
	}

	if (!draw.isTexture(img3)) {
		console.log("Loading Image 3");
		img3 = draw.createImage(hsv, 360, 100);
	}

	if (!draw.isTexture(mImg)) {
		console.log("Loading Modded Image");
		mImg = draw.createImage(mImgA, 128, 128);
	}

	if (!draw.isSpriteSheet(spriteSheet)) {
		console.log("Loading Sprite Sheet");
		spriteSheet = draw.openSpriteSheet("texture/spriteTest.json");
	}

	if (!draw.isFontLoaded("light")) {
		//console.log("Loading Font");
		draw.loadFont("light", "fonts/open_sans.json");
	}

	if (firstDraw) {
		console.log("Finished Loading");
		firstDraw = false;
	}

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
	draw.print(10, x++ * 30 + 40, "Supports Framebuffer: " + supportsFramebuffer);
	draw.print(10, x++ * 30 + 40, "Number of Processers: " + sys.numProcessers);

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

	draw.print(900, 240, "Modded Image");
	draw.draw(mImg, 900, 270, 100, 100);

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

sys.on("input", "test.input", function (e) {
	var key = e.key;
	var press = e.state === "press" || e.state === "repeat";

	console.log("[INPUT] : " + key + " : " + press);
});

sys.on("mouseButton", "test.mouseButton", function (e) {
	console.log("[MOUSE] : " + e.button + " : " + e.action + " : (" + e.x + ", " + e.y + ")");
});