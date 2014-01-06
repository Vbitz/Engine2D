// Started on Mon 6 Jan 2014 10:24:00pm
// First pass on rendering and spawning done at 10:45:40pm
// Second pass on rendering and spawning done at 10:58:40:pm
// Gameplay working at 11:11:25pm after 0hours 47mins 25seconds 

// Just a stock standed rythem game

var screenOffset = 32;

var colors = ["hotPink", "crimson", "lawnGreen", "deepSkyBlue"/*, "mediumPurple", "cadetBlue"*/],
	colorKeys = ["A", "S", "D", "F"],
	colorKeysLastScore = [0, 0, 0, 0],
	colorTokenSize = 48,
	colorPadding = 8,
	totalColorTokenSize = colorTokenSize + (colorPadding * 2);
	tokenSpeed = totalColorTokenSize * 2; // x pixels a second

var colorTokens = []; // format {row, y}

var score = 0,
	lives = 10;

for (var i = 0; i < colors.length; i++) {
	colorTokens[i] = [];
}

var spawnTimer = 0;

function lose() {
	sys.forceReload("script/fastGame1.js");
}

function randFloat(min, max) {
	if (max === undefined) {
		max = min;
		min = 0;
	}
	return (Math.random() * (max - min)) + min;
}

function rand(min, max) {
	return Math.floor(randFloat(min, max));
}

function spawnToken() {
	var row = rand(colors.length);
	var validRow = false;
	while (!validRow) {
		var rowArray = colorTokens[row];
		if (rowArray.length === 0) {
			validRow = true;
		} else {
			validRow = rowArray[rowArray.length - 1] > totalColorTokenSize + colorPadding;
			if (!validRow) {
				row = rand(colors.length);
			}
		}
	}

	colorTokens[row].push(0);
}

spawnToken();

sys.clearEvent("fastGame1.drawFunc");
sys.on("draw", "fastGame1.drawFunc", function () {
	// draw text
	draw.setColor(0xeaeaea);
	draw.setFont("basic", 16);
	draw.print(screenOffset, screenOffset, "Score: " + score);
	draw.print(screenOffset, screenOffset + 20, "Lives: " + lives);

	// try and spawn new tokens
	spawnTimer += sys.deltaTime;
	if (spawnTimer > 0.5) {
		spawnToken();
		spawnTimer = 0;
	}

	// draw gameboard
	var colorRectWidth = (colors.length * (colorTokenSize + (colorPadding * 2)));
	draw.setColor(0x404040);
	draw.rect((sys.screenWidth / 2) - (colorRectWidth / 2), 0, colorRectWidth, sys.screenHeight);

	// draw tokens and update logic
	var realTokenSpeed = tokenSpeed * sys.deltaTime;
	var tokenBase = (sys.screenWidth / 2) - (colorRectWidth / 2);
	for (var row = 0; row < colorTokens.length; row++) {
		draw.setColor(colors[row]);
		for (var i = 0; i < colorTokens[row].length; i++) {
			draw.rect(tokenBase + (totalColorTokenSize * row) + colorPadding,
						colorTokens[row][i] + colorPadding,
						colorTokenSize, colorTokenSize);
			colorTokens[row][i] += realTokenSpeed;
		}
		var zeroPos = colorTokens[row][0];
		if (input.keyDown(colorKeys[row])) {
			if (zeroPos > sys.screenHeight - (totalColorTokenSize * 2.5)
				&& zeroPos < sys.screenHeight - (totalColorTokenSize * 1)) {
				score += 100;
				colorTokens[row] = colorTokens[row].slice(1);
			}
		}
		if (zeroPos > sys.screenHeight) {
			colorTokens[row] = colorTokens[row].slice(1);
			lives--;
			if (lives < 1) {
				lose();
			}
		}
	}

	// draw score area
	draw.setColor(0x808080);
	draw.rect((sys.screenWidth / 2) - (colorRectWidth / 2), sys.screenHeight - (totalColorTokenSize * 2),
		colorRectWidth, 8);
	draw.rect((sys.screenWidth / 2) - (colorRectWidth / 2), sys.screenHeight - (totalColorTokenSize * 1),
		colorRectWidth, 8);

	// draw score helper keys
	draw.setColor(0x101010);
	draw.setFont("basic", 32);
	for (var row = 0; row < colorTokens.length; row++) {
		draw.print(tokenBase + (totalColorTokenSize * row) + 24,
						sys.screenHeight - (totalColorTokenSize * 2) + 24, colorKeys[row]);
	}
});