// wow this is really slow if you don't use the fast way

var IMAGE_WIDTH = 1024;
var IMAGE_HEIGHT = 1024;
var SCALE = 2;
var NOISE_SCALE = 0.03;
IMAGE_WIDTH = IMAGE_WIDTH / SCALE;
IMAGE_HEIGHT = IMAGE_HEIGHT / SCALE;

function setXY(array, x, y, z, value) {
	var index = 0;
	index += x * IMAGE_HEIGHT * 4;
	index += y * 4;
	index += z;
	array[index] = value;
}

function _noise(arr, level, width, height) {
	sys.perf("noiseGen", function () {
		noise.seed(Math.random());
		for (var x = 0; x < width; x++) {
			for (var y = 0; y < height; y++) {
				var val = Math.abs(noise.simplex2(x * NOISE_SCALE, y * NOISE_SCALE));
				setXY(arr, x, y, 0, val);
				setXY(arr, x, y, 1, val);
				setXY(arr, x, y, 2, val);
				setXY(arr, x, y, 3, 1.0);
			}
		}
	});
	return arr;
}

var noiseField = draw.createImageArray(IMAGE_WIDTH, IMAGE_HEIGHT);
noiseField = _noise(noiseField, 512, IMAGE_WIDTH, IMAGE_HEIGHT);

var img = draw.createImage(noiseField, IMAGE_WIDTH, IMAGE_HEIGHT);

sys.drawFunc(function () {
	draw.setColor("white");
	draw.draw(img, 0, 0, IMAGE_WIDTH * SCALE, IMAGE_HEIGHT * SCALE);
});

sys.keyboardFunc(function (a, k, p) {
	if (!p) return;
	if (k === "R") sys.forceReload("script/noise.js");
});