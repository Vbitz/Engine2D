sys.config("log_createImage", false);		// don't log to the console when we create a texture
sys.config("draw_createImageMipmap", false);// don't create mipmaps since it will fill the screen anyway

function rand(max) {
	return Math.floor(Math.random() * max);
}

var IMAGE_WIDTH = 1280;
var IMAGE_HEIGHT = 800;

function setXY(array, x, y, z, value) {
	var index = 0;
	index += x * IMAGE_HEIGHT * 4;
	index += y * 4;
	index += z;
	array[index] = value;
}

var drawArray = draw.createImageArray(IMAGE_WIDTH, IMAGE_HEIGHT);

drawArray[0] = 1;

var tmpTexture = 99999999;

sys.drawFunc(function () {
	draw.setColor("white");
	for (var i = 0; i < 600; i++) {
		setXY(drawArray, rand(IMAGE_WIDTH), rand(IMAGE_HEIGHT), rand(3), Math.random());
	}

	tmpTexture = draw.createImage(drawArray, IMAGE_WIDTH, IMAGE_HEIGHT, tmpTexture);
	draw.draw(tmpTexture, 0, 0, sys.screenWidth, sys.screenHeight);
});