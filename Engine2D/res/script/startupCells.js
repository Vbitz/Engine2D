// hello world this is a test, oh crud

var level = [];

var colors = {
	0: 0x00bfff,
	1: 0x888888,
	2: 0x7cfc00,
	3: 0x8b4518,
	4: 0x4169e1,
	5: 0xff0000
};

var levelMax = 10;
var levelMin = 260;
var maxDepth = 360;
var waterLevel = 60;
var lavaLevel = 200;

function rand(min, max) {
	return Math.floor(Math.random() * (max - min)) + min;
}

function genLevel() {
	var current = (levelMax + levelMin) / 2;
	for (var y = 0; y < 1001; y++) {
		if (current < levelMax) {
			current = levelMax;
		} else if (current > levelMin) {
			current = levelMin;
		}
		level[y] = [];
		for (var x = 0; x < waterLevel; x++) {
			level[y][x] = 0;
		}
		for (x = waterLevel; x < current; x++) {
			level[y][x] = 4;
		}
		if (current < waterLevel) {
			level[y][current] = 2;
		} else {
			level[y][current] = 4;
		}
		var dirtLevel = current + rand(4, 6);
		for (x = current + 1; x < dirtLevel; x++) {
			level[y][x] = 3;
		}
		for (x = dirtLevel; x < maxDepth; x++) {
			level[y][x] = rand(0, 8) === 0 ? 0 : 1;
		}
		if (rand(0, 100) > 15) {
			current += rand(0, 3) - 1;
		} else if (rand(0, 100) > 40) {
			current += rand(0, 10) - 5;
		} else if (rand(0, 100) > 50) {
			current += rand(0, 20) - 15;
		} else if (rand(0, 100) > 60) {
			current += rand(0, 40) - 15;
		} else if (rand(0, 100) > 90) {
			current += rand(0, 50) - 45;
		}
		current = Math.floor(current);
	}
}

function cells(factor) {
	var newLevel = level;
	for (var y = 1; y < level.length - 2; y++) {
		for (var x = 1; x < level[y].length - 2; x++) {
			var count = 0;
			count += level[y + 1][x];
			count += level[y - 1][x];
			count += level[y][x + 1];
			count += level[y][x - 1];
			count += level[y - 1][x - 1];
			count += level[y - 1][x + 1];
			count += level[y + 1][x + 1];
			count += level[y + 1][x - 1];
			if (count < x / factor) {
				newLevel[y][x] = 0;
			}
			if (count === 8 || count === 7) {
				newLevel[y][x] = 1;
			}
		}
	}
	level = newLevel;
}

function _draw() {
	for (var y = 0; y < level.length; y++) {
		for (var x = 0; x < level[y].length; x++) {
			draw.setColor(colors[level[y][x]]);
			draw.rect(y * 2, x * 2, 2, 2);
		}
	}
}

function _key(type, key, state) {
	if (key == "a") {
		genLevel();
	}
}

sys.drawFunc(_draw);
sys.keyboardFunc(_key);
genLevel();