sys.config("log_createImage", false);		// don't log to the console when we create a texture
sys.config("draw_createImageMipmap", false);// don't create mipmaps since it will fill the screen anyway
sys.config("log_profiler_maxTime", false);

draw.setFont("basic", 12);

var screenWidth = 512;
var screenHeight = 512;

var drawArray = draw.createImageArray(screenWidth, screenHeight);

function setArray(arr, x, y, z, value) {
	var pos = 0;
	pos += y * screenWidth * 4;
	pos += x * 4;
	pos += z;
	arr[pos] = value;
}

function setPixel(x, y, r, g, b) {
	if (r > 1 || g > 1 || b > 1) {
		r = r / 256;
		g = g / 256;
		b = b / 256;
	}
	setArray(drawArray, x, y, 0, r);
	setArray(drawArray, x, y, 1, g);
	setArray(drawArray, x, y, 2, b);
}

function rand(f, min, max) {
	var val = 0;
	if (max === undefined) {
		val = Math.random() * min;
	} else {
		val = Math.random() * (max - min) + min;
	}
	return f ? val : Math.floor(val);
}

function rect(xStart, yStart, width, height, r, g, b, randOffset) {
	if (r > 1 || g > 1 || b > 1) {
		r = r / 256;
		g = g / 256;
		b = b / 256;
	}
	var baseR = r;
	var baseG = g;
	var baseB = b;
	for (var x = xStart; x < (xStart + width); x++) {
		for (var y = yStart; y < (yStart + height); y++) {
			if (randOffset) {
				var randVal = rand(true, -0.01, 0.01);
				r = baseR + randVal;
				g = baseG + randVal;
				b = baseB + randVal;
			}
			setPixel(x, y, r, g, b);
		}
	}
}

draw.setColor("white");

sys.drawFunc(function () {
	rect(0, 0, screenWidth, screenHeight, 0.9, 0.9, 0.9, false);

	rect(10, 30, 10, 10, 0.6, 0.5, 0.9, true);
	rect(30, 10, 10, 10, 0.9, 0.2, 0.3, true);
	rect(10, 10, 10, 10, 0.2, 0.2, 0.2, true);
	rect(30, 30, 10, 10, 0.6, 0.5, 0.9, true);

	var texID = draw.createImage(drawArray, screenWidth, screenHeight);
	draw.draw(texID, 0, 10, sys.screenWidth, sys.screenHeight - 10);
	draw.freeImage(texID);
});