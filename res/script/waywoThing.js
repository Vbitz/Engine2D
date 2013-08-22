fs.configDir("waywoThing");

sys.config("log_createImage", false);		// don't log to the console when we create a texture
sys.config("draw_createImageMipmap", false);// don't create mipmaps since it will fill the screen anyway

function rand(max) {
	return Math.floor(Math.random() * max);
}

var IMAGE_WIDTH = 512;
var IMAGE_HEIGHT = 512;

function setXY(array, y, x, z, value) {
	var index = 0;
	index += x * IMAGE_HEIGHT * 4;
	index += y * 4;
	index += z;
	array[index] = value;
}

var drawArray = draw.createImageArray(IMAGE_WIDTH, IMAGE_HEIGHT);

drawArray[0] = 1;

var colWidth = 16;

for (var i = 0; i < IMAGE_WIDTH / colWidth; i++) {
	var baseColorR = 1.0 - Math.random() / 4;
	var baseColorG = 1.0 - Math.random() / 4;
	var baseColorB = 1.0 - Math.random() / 4;
	for (var x = 0; x < colWidth; x++) {
		for (var y = 0; y < IMAGE_HEIGHT; y++) {
			setXY(drawArray, x + (i * colWidth), y, 0, baseColorR - (y / IMAGE_HEIGHT) + (Math.random() / 10) - 0.05);
			setXY(drawArray, x + (i * colWidth), y, 1, baseColorG - (y / IMAGE_HEIGHT) + (Math.random() / 10) - 0.05);
			setXY(drawArray, x + (i * colWidth), y, 2, baseColorB - (y / IMAGE_HEIGHT) + (Math.random() / 10) - 0.05);
		}
	}
}

sys.drawFunc(function () {
	var tmpTexture = draw.createImage(drawArray, IMAGE_WIDTH, IMAGE_HEIGHT);
	draw.draw(tmpTexture, 0, 0, sys.screenWidth, sys.screenHeight);
	draw.freeImage(tmpTexture);
});

sys.keyboardFunc(function (type, key, press) {
	if (press && key === "S") {
		sys.saveScreenshot("Screenshot.png");
	}
});