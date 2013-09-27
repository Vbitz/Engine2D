fs.configDir("minesweeper");

var levelWidth = 30,
	levelHeight = 20,
	bombs = Math.floor((levelWidth * levelHeight) / 8),
	tileSize = 16,
	xOff = 20,
	yOff = 40,
	levelDrawWidth = levelWidth * (tileSize + 1) + xOff,
	levelDrawHeight = levelHeight * (tileSize + 1) + yOff,
	cursorSize = tileSize / 2,
	lastMouse = false;

var colorPalate = {
	background: 0x101010,
	text: 0xeaeaea,
	cursor0: 0x101010,
	cursor1: 0x101010,
	0x5ee2: 0xaaaaaa, // unseen
	0: "white",
	1: "darkSlateBlue",
	2: "darkRed",
	3: "darkGreen",
	4: "mediumVioletRed",
	5: "midnightBlue",
	6: "darkSlateGray",
	7: "teal",
	8: "darkMagenta",
	0xfa46: 0xadabcc, // flag
	0xb077b: 0xaaabcc, // bomb
};

var level = [];
var drawLevel = [];

function lose() {
	sys.forceReload("script/minesweeper.js");
}

function win() {
}

function rand(min, max) {
	if (max === undefined) {
		return Math.floor(Math.random() * min);
	} else {
		return Math.floor(Math.random() * (max - min)) + min;
	}
}

function genLevel() {
	level = [];
	drawLevel = [];

	for (var x = 0; x < levelWidth; x++) {
		level[x] = [];
		drawLevel[x] = [];
	
		for (var y = 0; y < levelHeight; y++) {
			level[x][y] = 0;
			drawLevel[x][y] = 0x5ee2;
		}
	}
	for (var x = 0; x < bombs; x++) {
		var posX = rand(levelWidth);
		var posY = rand(levelHeight);
	
		if (getBlock(false, posX, posY) == 0xb077b) {
			x--;
		} else {
			level[posX][posY] = 0xb077b;
	
			incBlock(posX - 1, posY    );
			incBlock(posX + 1, posY    );
			incBlock(posX    , posY - 1);
			incBlock(posX    , posY + 1);
			incBlock(posX + 1, posY - 1);
			incBlock(posX + 1, posY + 1);
			incBlock(posX - 1, posY + 1);
			incBlock(posX - 1, posY - 1);
		}
	}
}

function getBlock(draw, x, y) {
	if (draw) {
		return drawLevel[x][y];
	} else {
		return level[x][y];
	}
}

function incBlock(x, y) {
	if (x < 0 || x >= levelWidth) return;
	if (y < 0 || y >= levelHeight) return;
	
	if (level[x][y] === 0xb077b) return;
	
	level[x][y]++;
}

function setBlock(x, y, value) {
	if (x < 0 || x >= levelWidth) return;
	if (y < 0 || y >= levelHeight) return;
	
	level[x][y] = value;
}

function revealBlock(x, y) {
	if (x < 0 || x >= levelWidth) return;
	if (y < 0 || y >= levelHeight) return;

	if (level[x][y] === 0xb077b) return;

	if (drawLevel[x][y] === level[x][y]) return;

	drawLevel[x][y] = level[x][y];
	
	if (level[x][y] === 0) {
		revealBlock(x - 1, y    );
		revealBlock(x + 1, y    );
		revealBlock(x    , y - 1);
		revealBlock(x    , y + 1);
		revealBlock(x + 1, y - 1);
		revealBlock(x + 1, y + 1);
		revealBlock(x - 1, y + 1);
		revealBlock(x - 1, y - 1);
	}
}

function clickBlock(x, y) {
	if (getBlock(false, x, y) == 0xb077b) {
		lose();
	} else {
		revealBlock(x, y);
	}
}

function clickBlockMouse(x, y) {
	if (x < xOff || x > levelDrawWidth) return;
	if (y < yOff || y > levelDrawHeight) return;

	clickBlock(Math.floor((x - xOff) / (tileSize + 1)), Math.floor((y - yOff) / (tileSize + 1)));
}

function drawCursor(x, y) {
	draw.setColor(colorPalate.cursor0);
	draw.rect(x - cursorSize, y, cursorSize * 2 + 1, 1);
	draw.rect(x, y - cursorSize, 1, cursorSize * 2 + 1);
}

draw.clearColor(colorPalate.background);

genLevel();

sys.drawFunc(function () {
	draw.setFont("basic", 12);
	draw.setColor(colorPalate.text);
	draw.print(5, 20, "Bombs left: " + bombs);

	for (var x = 0; x < levelWidth; x++) {
		for (var y = 0; y < levelHeight; y++) {
			var xLoc = xOff + (x * (tileSize + 1));
			var yLoc = yOff + (y * (tileSize + 1));

			var b = getBlock(true, x, y); 
			
			if (b > 0 && b < 9) {
				draw.setColor(colorPalate[0]);
				draw.rect(xLoc, yLoc, tileSize, tileSize);

				draw.setColor(colorPalate[b]);
				draw.print(xLoc + 2, yLoc + 2, b.toString(10));
			} else {
				draw.setColor(colorPalate[getBlock(true, x, y)]);
				draw.rect(xLoc, yLoc, tileSize, tileSize);
			}
		}
	}

	drawCursor(input.mouseX, input.mouseY);
	
	if (input.leftMouseButton && !lastMouse) {
		clickBlockMouse(input.mouseX, input.mouseY);
		lastMouse = true;
	} else if (!input.leftMouseButton) {
		lastMouse = false;
	}
});

sys.keyboardFunc(function (action, key, press) {
	if (key === "R" && press) {
		genLevel();
	} else if (key === "S" && press) {
		sys.saveScreenshot("screen.png");
	}
});