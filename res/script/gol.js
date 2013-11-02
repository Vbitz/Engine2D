fs.configDir("GameOfLife");

var width = 320;
var height = 188;

var paused = false;

var items = [];

function regen() {
	width = sys.screenWidth / 4;
	height = sys.screenHeight / 4;
	for (var x = 0; x < width; x++) {
		items[x] = [];
		for (var y = 0; y < height; y++) {
			items[x][y] = false;
		}
	}
}

regen();

function get(x, y) {
	return items[x][y];
}

function set(x, y, i) {
	items[x][y] = i;
}

sys.drawFunc(function () {
	var mx = Math.floor(input.mouseX / 4);
	var my = Math.floor(input.mouseY / 4);
	if (input.leftMouseButton) {
		if (items[mx] !== undefined && items[mx][my] !== undefined) {
			set(mx, my, Math.floor(Math.random() * 255 * 255 * 255));
		}
	} else {
		draw.grid(mx * 4, my * 4, 4, 4);
	}
	for (var x = 1; x < width - 1; x++) {
		for (var y = 1; y < height - 1; y++) {
			if (!paused) {
				var val = 0;

				val += get(x + 1, y) !== false;
				val += get(x - 1, y) !== false;
				val += get(x, y + 1) !== false;
				val += get(x, y - 1) !== false;

				val += get(x + 1, y + 1) !== false;
				val += get(x + 1, y - 1) !== false;
				val += get(x - 1, y + 1) !== false;
				val += get(x - 1, y - 1) !== false;

				if (get(x, y) && (val < 2 || val > 3)) {
					set(x, y, false);
				}
				if (!get(x, y) && val === 3) {
					var avr = 0;

					avr += get(x + 1, y);
					avr += get(x - 1, y);
					avr += get(x, y + 1);
					avr += get(x, y - 1);

					avr += get(x + 1, y + 1);
					avr += get(x + 1, y - 1);
					avr += get(x - 1, y + 1);
					avr += get(x - 1, y - 1);

					set(x, y, Math.floor(avr / 3 + (Math.random() * 20)));
				}
			}

			if (get(x, y) !== false) {
				draw.setColor(get(x, y));
				draw.rect(x * 4, y * 4, 4, 4);
			}
		}
	}
});

sys.keyboardFunc(function (type, key, press) {
	if (press && key === "ESC") {
		sys.exit();
	} else if (press && key === "R") {
		regen();
	} else if (press && key === "F") {
		sys.toggleFullscreen();
	} else if (press && key === " ") {
		paused = !paused;
	} else if (press && key === "S") {
		sys.saveScreenshot(Math.floor(Math.random() * 100000).toString(16) + ".png");
	}
});