fs.configDir("testSuite");

var tests = {
	"Filesystem": function () {
		fs.writeFile("testing.txt", "Hello world");
		return fs.readFile("testing.txt") === "Hello world";
	},
	"Database": function () {
		db.open("test.db");

		db.exec("DROP TABLE IF EXISTS testing");
		db.exec("CREATE TABLE IF NOT EXISTS testing(t1 TEXT)");
		db.exec("INSERT INTO testing VALUES('hello world')");

		return db.execPrepare("SELECT * FROM testing")[0].t1 === "hello world";
	}
};

var testResults = {};

for (var i in tests) {
	try {
		testResults[i] = tests[i]();
	} catch (e) {
		testResults[i] = false;
	}
}

draw.clearColor(0.01, 0.01, 0.01);

var img = draw.openImage("image/testing.png");

var pix = [];

for (var i = 0; i < 100 * 100 * 3; i++) {
	pix[i] = Math.cos(i);
}

var img2 = draw.createImage(pix, 100, 100);

var t = 0;

sys.drawFunc(function () {
	draw.grad(0, 5, 600, 35, 0xfafafa, 0x000000, false);
	draw.setColor(0x000000);
	draw.print(10, 15, "Engine2D Test Suite");
	draw.setColor(0xffffff);
	draw.print(10, 50, "Automated Tests");
	var x = 2;
	for (var i in testResults) {
		if (testResults[i]) {
			draw.setColor(0x00ff00);
		} else {
			draw.setColor(0xff0000);
		}
		draw.rect(15, x * 40, 220, 30);
		draw.setColor(0x000000);
		draw.print(20, x * 40 + 5, i);
		x++;
	}
	draw.setColor(0xffffff);
	draw.print(400, 50, "Rendering Tests");
	draw.print(420, 80, "Rect");
	draw.rect(410, 100, 100, 100);
	draw.print(540, 80, "Grid");
	draw.grid(530, 100, 100, 100);
	draw.print(650, 80, "Gradents");
	draw.grad(650, 100, 100, 100, 0xffffff, 0x000000, true);
	draw.grad(770, 100, 100, 100, 0xffffff, 0x000000, false);
	draw.print(400, 220, "PNG Image");
	draw.draw(img, 410, 250, 100, 100);
	draw.print(520, 220, "Gen Image");
	draw.drawSub(img2, 530, 250, 512, 400, t, t += 0.1, 16, 16);
});

sys.keyboardFunc(function (type, key, press) {
	if (key === "S" && press) {
		sys.saveScreenshot("testing.png");
	}
});