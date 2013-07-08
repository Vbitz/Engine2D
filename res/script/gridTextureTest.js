var img = img ? img : -1;

var scale = 1.0;

var scaleDirection = 0.01;

sys.drawFunc(function () {
	if (!draw.isTexture(img)) {
		img = draw.createImage([0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
								1.0, 1.0, 1.0, 0.0, 0.0, 0.0], 2, 2);
	}

	draw.drawSub(img, 0, 0, sys.screenWidth, sys.screenHeight, 0, 0, sys.screenWidth * scale, sys.screenHeight * scale);

	draw.setColor("black");

	draw.print(20, 20, "scale: " + scale);

	draw.setColor("white");

	draw.print(19, 19, "scale: " + scale);

	scale += scaleDirection;

	if (scale > 1.5 || scale < 0.25) {
		scaleDirection = -scaleDirection;
	}
});