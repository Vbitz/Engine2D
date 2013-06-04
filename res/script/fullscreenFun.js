var lines = fs.readFile("content/testFullscreen.txt").replace(/\t/g, "    ").split('\n');

sys.drawFunc(function () {
	draw.setColor(0xffffff);
	for (var i = 0; i < lines.length; i++) {
		draw.print(2, i * 18 + 2, lines[i]);
	}
});

sys.keyboardFunc(function (type, key, state) {
	if (state && key === "F") {
		sys.toggleFullscreen();
	}
});