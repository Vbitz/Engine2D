// Based on https://imgur.com/a/QWMGi

var regex = /.*(12|21).*/;

var gridSize = 2048,
	scale = 0.5;

var black = 0.0,
	white = 1.0;

function make2DArray(w, h, val) {
	var ret = [];
	for (var x = 0; x < w; x++) {
		ret[x] = [];
		for (var y = 0; y < h; y++) {
			ret[x][y] = val;
		}
	}
	return ret;
}

function arrayRect(arr, x, y, w, h, val) {
	for (var xi = x; xi < (x + w); xi++) {
		for (var yi = y; yi < (y + h); yi++) {
			arr[xi][yi] += val;
		}
	}
}

function makeGrid(s) {
	var ret = make2DArray(s, s, "");
	var currentTileSize = s / 2;
	while (currentTileSize > 1) {
		for (var x = 0; x < s; x += currentTileSize) {
			for (var y = 0; y < s; y += currentTileSize) {
				var xEven = ((x / currentTileSize) % 2) === 0,
					yEven = ((y / currentTileSize) % 2) === 0;
				var num = 0;
				if		( xEven &&  yEven) num = 4;
				else if ( xEven && !yEven) num = 1;
				else if (!xEven &&  yEven) num = 3;
				else if (!xEven && !yEven) num = 2;
				arrayRect(ret, x, y, currentTileSize, currentTileSize, num.toString(10));
			}
		}
		currentTileSize = currentTileSize / 2;
	}
	return ret;
}

function computeGrid(g, s) {
	var colorGrid = [];
	for (var x = 0; x < s; x++) {
		colorGrid[x] = [];
		for (var y = 0; y < s; y++) {
			if (regex.test(g[x][y])) {
				colorGrid[x][y] = black;
			} else {
				colorGrid[x][y] = white;
			}
		}
	}
	return colorGrid;
}

function setXY(array, x, y, z, value) {
	var index = 0;
	index += x * gridSize * 4;
	index += y * 4;
	index += z;
	array[index] = value;
}

function renderGrid(g, gSize) {
	var img = draw.createImageArray(gSize, gSize);
	for (var x = 0; x < gSize; x++) {
		for (var y = 0; y < gSize; y++) {
			setXY(img, x, y, 0, g[x][y]);
			setXY(img, x, y, 1, g[x][y]);
			setXY(img, x, y, 2, g[x][y]);
			setXY(img, x, y, 3, 1.0);
		}
	}
	return draw.createImage(img, gSize, gSize);
}

var grid = makeGrid(gridSize);

grid = computeGrid(grid, gridSize);

var gridImg = renderGrid(grid, gridSize);

sys.on("draw", "regexFractal.draw", function (e) {
	draw.draw(gridImg, 0, 0, gridSize * scale, gridSize * scale);
});