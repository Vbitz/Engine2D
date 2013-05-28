function noise(arr, level, width, height) {
	if (level < 1) {
		return arr;
	}
	console.log(level);
	for (var x = 0; x < width; x += level) {
		for (var y = 0; y < height; y += level) {
			if (arr[x][y] === 0) {
				arr[x][y] = level + (Math.random() * 50);
			}
		}
	}
	return noise(arr, Math.floor(level / 2), width, height);
}

function doNoise(level, width, height) {
	var items = [];
	for (var x = 0; x < width; x++) {
		items[x] = [];
		for (var y = 0; y < height; y++) {
			items[x][y] = 0;
		}
	}
	return noise(items, level, width, height);
}

var noiseField = doNoise(128, 1000, 700);

sys.drawFunc(function () {
	for (var x = 0; x < 1000; x++) {
		for (var y = 0; y < 700; y++) {
			draw.setColorI(noiseField[x][y], noiseField[x][y], noiseField[x][y]);
			draw.rect(x * 1, y * 1, 1, 1);
		}
	}
});