sys.config("core.debug.profiler", true);
sys.config("core.script.autoReload", true);

fs.configDir("snakeAI");

draw.clearColor(0xdadada);

// Snake game, started at Sat 4th Jan 2014 1:04pm NZST
// Player movement working at 1:39pm NZST
// Player shrinking at 1:42pm NZST
// Version 1 of game finished at 2:00pm NZST
// Version 2 of game finished at 3:52pm NZST (after a break) 
// Begin writing AI, started at Fri 10th Jan 2014 2:07pm NZST 
// AI Movement working at 2:16pm NZST
// AI Finished at 2:37pm NZST

var levelSize = 32,
	tileSize = 16,
	levelOffset = 32,
	startingScoreTiles = 60;

var upKey = "W",
	downKey = "S",
	leftKey = "A",
	rightKey = "D";

// start the player moving up
var playerDirectionX = 0,
	playerDirectionY = -1;

var playerTiles = [];
var scoreTiles = [];

var playerHeadX = levelSize / 2,
	playerHeadY = levelSize / 2;

var playerTargetSize = 5, // start at 5 size
	playerScoreSizePlus = 2; // add 2 each time the player gets points

var updatesPerSecond = 600;

var steps = 0;

var moveSteps = 1,
	currentMoveStep = 0;

var scores = [];

if (fs.fileExists("scores.json")) {
	scores = JSON.parse(fs.readFile("scores.json"));
}

function addScore(time, steps, score) {
	var scoreObj = {
		time: time.toString().substring(0, 24),
		score: score,
		steps: steps
	};

	if (scores.length === 0) {
		scores.splice(0, 0, scoreObj);
	} else {
		var addedScore = false;
		for (var i = 0; i < scores.length; i++) {
			if (score > scores[i].score) {
				scores.splice(i, 0, scoreObj);
				addedScore = true;
				break;
			}
		}
		if (!addedScore) {
			scores.push(scoreObj);
		}
	}
	fs.writeFile("scores.json", JSON.stringify(scores, 0, '\t'));
}

function lose() {
	addScore(new Date(), steps, playerTiles.length * 100);
	sys.forceReload("script/snakeAI.js");
}

// Just some cute utility functions

function rand(min, max) {
	if (max === undefined) {
		max = min;
		min = 0;
	}
	return Math.random() * (max - min) + min;
}

function randFloor(min, max) {
	return Math.floor(rand(min, max));
}

function cloneArray(a) {
	var ret = [];
	for (var i = 0; i < a.length; i++) ret[i] = a[i];
	return ret;
}

// function based on
//+ Jonas Raoni Soares Silva
//@ http://jsfromhell.com/array/shuffle [v1.0]
function shuffleArray(a) {
	var ret = cloneArray(a);
	var swapIndex,
		x,
		i = ret.length;
	while (i > 0) { // go though the array swaping values
		swapIndex = randFloor(i);
		x = ret[--i];
		ret[i] = ret[swapIndex];
		ret[swapIndex] = x;
	}
	return ret;
}

var level = [];

function addScoreTile() {
	var tileX = Math.floor(Math.random() * (levelSize - 3)) + 1,
		tileY = Math.floor(Math.random() * (levelSize - 3)) + 1;
	while (checkColision(tileX, tileY, true)) {
		tileX = Math.floor(Math.random() * (levelSize - 3)) + 1;
		tileY = Math.floor(Math.random() * (levelSize - 3)) + 1;
	}
	scoreTiles[tileX][tileY] = true;
	level[tileX][tileY] = 2;
}

function genLevel() {
	level = [];
	for (var x = 0; x < levelSize; x++) {
		level[x] = [];
		scoreTiles[x] = [];
		for (var y = 0; y < levelSize; y++) {
			level[x][y] = 0;
			scoreTiles[x][y] = false;
		}
	}
	level[playerHeadX][playerHeadY] = 1;
	playerTiles.push([playerHeadX, playerHeadY]);
	for (var i = 0; i < startingScoreTiles; i++) {
		addScoreTile();	
	}
}

function checkColision(x, y, dry, ai) {
	if (x < 0 || x > levelSize - 3 || y < 0 || y > levelSize - 3) return true;
	if (level[x][y] === 2 && !dry) {
		playerTargetSize += playerScoreSizePlus;
		level[x][y] = 0;
		scoreTiles[x][y] = false;
		addScoreTile();
		return false;
	}
	if (!ai) {
		return level[x][y] > 0;
	} else {
		return level[x][y] != 2;
	}
}

function movePlayer(x, y) {
	if (playerDirectionX == x || playerDirectionY == y)		return;
	if (-playerDirectionX == x || -playerDirectionY == y)	return;
		
	playerDirectionX = x;
	playerDirectionY = y;
}

genLevel();

var updateTimer = 0;

sys.drawFunc(function () {
	draw.setFont("basic", 12);
	draw.setColor(0x202020);
	draw.rect(levelOffset, levelOffset, levelSize * tileSize, levelSize * tileSize);
	for (var x = 0; x < levelSize; x++) {
		for (var y = 0; y < levelSize; y++) {
			if (level[x][y] !== 0) {
				if (level[x][y] === 1) {
					draw.setColor(0xdadada); // player
				} else if (level[x][y] === 2) {
					draw.setColor(0xffd700); // coin
				} else if (level[x][y] === 3) {
					draw.setColor(0xff0000); // losePos
				}
				draw.rect(levelOffset + tileSize + (x * tileSize) + 1,
							levelOffset + tileSize + (y * tileSize) + 1,
							tileSize - 2, tileSize - 2);
			}
		}
	}

	draw.setColor(0x202020);

	var scoreStartX = levelOffset + 16 + levelSize * tileSize;

	var scoreY = levelOffset * 2;

	for (var i = 0; i < scores.length; i++) {
		draw.print(scoreStartX, scoreY, "#" + (i + 1) + " | " + scores[i].time + " | " + scores[i].score + " | " + scores[i].steps);
		scoreY += 16;
		if (i > 20) {
			break;
		}
	}

	draw.print(levelOffset + 16, levelSize * tileSize + levelOffset + 16, "Score: " + (playerTiles.length * 100));

	updateTimer += sys.deltaTime;
	if (updateTimer > 1 / updatesPerSecond) {
		updateTimer = 0;
		// tick time
		
		steps++;

		// were a AI, let's select a movement that would not kill us
		var movements = [[0, -1], [0, 1], [-1, 0], [1, 0]];
		movements = shuffleArray(movements);

		// TODO: 100 snakes

		currentMoveStep++;
		if (currentMoveStep > moveSteps) {
			playerDirectionX = movements[0][0];
			playerDirectionY = movements[0][1];
			currentMoveStep = 0;
		}

		if (checkColision(playerHeadX + playerDirectionX, playerHeadY + playerDirectionY, true, true)) {
			for (var i = 0; i < movements.length; i++) {
				var move = movements[i];
				if (!checkColision(playerHeadX + move[0], playerHeadY + move[1], true)) {
					playerDirectionX = move[0];
					playerDirectionY = move[1];
					break;
				}
			}
		}

		if (checkColision(playerHeadX + playerDirectionX, playerHeadY + playerDirectionY)) {
			console.log("lose");
			lose();
		} else {
			// move the player head to the new posion
			playerHeadX += playerDirectionX;
			playerHeadY += playerDirectionY;
			level[playerHeadX][playerHeadY] = 1;
			playerTiles.push([playerHeadX, playerHeadY]);
			if (playerTiles.length > playerTargetSize) {
				level[playerTiles[0][0]][playerTiles[0][1]] = 0;
				playerTiles = playerTiles.slice(1);
			}
		}
	}
});