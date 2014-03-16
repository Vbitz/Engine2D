/* jshint esnext: true */

function* rainbow() {
	while (true) {
		for (var i = 0; i < 360; i += 10) {
			yield draw.getRGBFromHSV(i, 1.0, 1.0);
		}
	}
}

sys.on("draw", "cameraTest2.draw", function () {
	var rainbowGen = rainbow();
	draw.cameraReset();
	draw.cameraPan(sys.screenWidth / 2, sys.screenHeight / 2);
	for (var i = 0; i < 70; i++) {
		draw.setColor(rainbowGen.next().value);
		draw.rect(-10, -10, 20, 20);
		draw.cameraRotate(44);
		draw.cameraPan(0, 120);
	}
});