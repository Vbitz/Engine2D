fs.configDir("testSuite");

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
	}
};

var testResults = {};

for (var i in tests) {
	try {
		sys.perf(i + "_test", function () {
			testResults[i] = tests[i]();
		});
	} catch (e) {
		testResults[i] = false;
	}
}

draw.clearColor(0x010101);

var pix = [];

for (var i = 0; i < 100 * 100 * 3; i++) {
	pix[i] = Math.cos(i);
}

var hsv = [];

var point = 0;

for (var x = 0; x < 360; x++) {
	for (var y = 0; y < 100; y++) {
		var rgb = draw.getRGBFromHSV(x, 1.0, y / 100);
		hsv[point++] = rgb.r;
		hsv[point++] = rgb.g;
		hsv[point++] = rgb.b;
	}
}

var img = img ? img : draw.openImage("image/testing.png");
var img2 = img2 ? img2 : draw.createImage(pix, 100, 100);
var img3 = img3 ? img3 : draw.createImage(hsv, 360, 100);

var t = 0;

var supportsFramebuffer = sys.hasExtention("GL_ARB_framebuffer_object");

console.log("Loading Test Suite");

var firstDraw = true;

sys.drawFunc(function () {
	if (!draw.isTexture(img)) {
		console.log("Loading Image 1");
		img = draw.openImage("image/testing.png");
	}

	if (!draw.isTexture(img2)) {
		console.log("Loading Image 2");
		img2 = draw.createImage(pix, 100, 100);
	}

	if (!draw.isTexture(img3)) {
		console.log("Loading Image 3");
		img3 = draw.createImage(hsv, 360, 100);
	}

	if (!draw.isFontLoaded("light")) {
		console.log("Loading Font");
		draw.loadFont("light", "fonts/OpenSans-Light.ttf");
	}

	if (firstDraw) {
		console.log("Finished Loading");
		firstDraw = false;
	}

	draw.grad(0, 25, 600, 35, 0xfafafa, 0x000000, false);
	draw.setColor(0x000000);
	draw.setFont("basic", 16);
	draw.print(10, 35, "Engine2D Test Suite");

	draw.setColor(0xffffff);
	draw.print(10, 70, "Automated Tests");
	draw.print(400, 70, "Rendering Tests");

	draw.setFont("basic", 12);
	var x = 2;
	for (var i in testResults) {
		if (testResults[i]) {
			draw.setColor(0x00ff00);
		} else {
			draw.setColor(0xff0000);
		}
		draw.rect(15, x * 30 + 35, 220, 20);
		draw.setColor(0x000000);
		draw.print(20, x * 30 + 40, i);
		x++;
	}

	draw.setColor(0xffffff);
	draw.print(10, x++ * 30 + 40, "Supports Framebuffer: " + supportsFramebuffer);

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
	draw.setColor("white");
	draw.draw(img, 410, 270, 100, 100);

	draw.print(520, 240, "Gen Image");
	draw.drawSub(img2, 530, 270, 100, 100, t, t += 0.1, 16, 16);

	draw.print(640, 240, "Tinted Image");
	draw.setColor(Math.floor(Math.random() * 255 * 255 * 255));
	draw.draw(img, 650, 270, 100, 100);

	draw.setColor("white");
	draw.print(760, 240, "Circle");
	draw.circle(770 + 50, 270 + 50, 50, 40, 200, 0.1, 1.0, true);

	for (var i = 0; i < 20; i++) {
		draw.curve(i * 100, 500, i * 100, 600, 850, 600, input.mouseX, input.mouseY);
	}

	draw.print(400, 400, "HSV Image");
	draw.draw(img3, 410, 420, 360, 100);
});

sys.keyboardFunc(function (type, key, press) {
	console.log("[" + type + "] : " + key + " : " + press);
	if (key === "S" && press) {
		sys.saveScreenshot("testing.png");
	} else if (key === "F" && press) {
		sys.toggleFullscreen();
	}
});