sys.drawFunc(function () {
	draw.rect(100, 100, 200, 200);
});

sys.keyboardFunc(function (type, key, press) {
	if (key === " ") {
		sys.toggleFullscreen();
	} else if (key === "ESC") {
		sys.exit();
	}
});