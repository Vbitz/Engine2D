sys.drawFunc(function () {
	//console.log("Frame");
	draw.setColor("red");
	draw.rect(100, 100, 20, 80);
	draw.setColor("blue");
	draw.rect(100, 200, 20, 20);
	draw.setColor("green");
	draw.rect(100, 300, 80, 80);
});

sys.keyboardFunc(function (type, key, press) {
	if (key === "G") {
		sys.config("cl_openGL3", true);
		sys.restartRenderer();
	} else if (key === "ESC") {
		sys.exit();
	}
});