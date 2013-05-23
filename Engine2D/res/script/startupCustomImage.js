var arr = [];

var size = 512;

/*for (var x = 0; x < size; x++) {
	for (var y = 0; y < size; y++) {
		arr[(x * size + y) * 3 + 0] = Math.sin(x);
		arr[(x * size + y) * 3 + 1] = Math.sin(y);
		arr[(x * size + y) * 3 + 2] = Math.sin(y);
	}
}*/

for (var i = 0; i < 512 * 512 * 3; i++) {
	arr[i] = Math.random();
}

var img = null;

try {
	img = draw.createImage(arr, size, size);
} catch (e) {
	if (e.stack) {
		console.log(e.stack);
	}
}

sys.drawFunc(function () {
	draw.draw(img, 10, 10, 256, 256);
});