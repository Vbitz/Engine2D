fs.configDir("snake");

draw.clearColor(0xdadada);

// Snake game, started at Sat 4th Jan 2014 1:04pm NZST
// Player movement working at 1:39pm NZST
// Player shrinking at 1:42pm NZST
// Version 1 of game finished at 2:00pm NZST
// Version 2 of game finished at 3:52pm NZST (after a break) 

var levelSize = 32,
	tileSize = 16,
	levelOffset = 32,
	startingScoreTiles = 2;

var upKey = "W",
	downKey = "S",
	leftKey = "A",
	rightKey = "D";

// start the player moving up
var playerDirectionX = 0,
	playerDirectionY = -1;

var directionChanged = false;

var playerTiles = [];
var scoreTiles = [];

var playerHeadX = levelSize / 2,
	playerHeadY = levelSize / 2;

var playerTargetSize = 5, // start at 5 size
	playerScoreSizePlus = 2; // add 2 each time the player gets points

var updatesPerSecond = 24;

var minEndTime = 0.3,
	currentEndTime = 0,
	lost = false;

var scores = [];

if (fs.fileExists("scores.json")) {
	scores = JSON.parse(fs.readFile("scores.json"));
}

function addScore(time, score) {
	var scoreObj = {
		time: time.toString().substring(0, 24),
		score: score
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
	draw.clearColor(0xffbbbb);
	lost = true;
	var losePosX = playerHeadX + playerDirectionX,
		losePosY = playerHeadY + playerDirectionY;
	if (losePosX < 0 || losePosX > levelSize - 3 || losePosY < 0 || losePosY > levelSize - 3) {
		losePosX = playerHeadX;
		losePosY = playerHeadY;
	}
	level[losePosX][losePosY] = 3;
	addScore(new Date(), playerTiles.length * 100);
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

function checkColision(x, y, dry) {
	if (x < 0 || x > levelSize - 3 || y < 0 || y > levelSize - 3) return true;
	if (level[x][y] === 2 && !dry) {
		playerTargetSize += playerScoreSizePlus;
		level[x][y] = 0;
		scoreTiles[x][y] = false;
		addScoreTile();
		return false;
	}
	return level[x][y] > 0;
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

	draw.print(levelOffset + 16, levelSize * tileSize + levelOffset + 16, "Score: " + (playerTiles.length * 100));

	// render scores

	var scoreStartX = levelOffset + 16 + levelSize * tileSize;

	var scoreY = levelOffset * 2;

	for (var i = 0; i < scores.length; i++) {
		draw.print(scoreStartX, scoreY, "#" + (i + 1) + " | " + scores[i].time + " | " + scores[i].score);
		scoreY += 16;
		if (i > 13) {
			break;
		}
	}

	if (lost) {
		currentEndTime += sys.deltaTime;
		draw.print(levelOffset + 16, levelSize * tileSize + levelOffset + 24, "You lose. Press any key to restart");
		return;
	}

	updateTimer += sys.deltaTime;
	if (updateTimer > 1 / updatesPerSecond) {
		updateTimer = 0;
		// tick time
		directionChanged = false;
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

function movePlayer(x, y) {
	if (playerDirectionX == x || playerDirectionY == y)		return;
	if (-playerDirectionX == x || -playerDirectionY == y)	return;
		
	playerDirectionX = x;
	playerDirectionY = y;
}

sys.keyboardFunc(function (_, key, press) {
	if (lost) {
		if (currentEndTime > minEndTime) {
			sys.forceReload("script/snake.js");
		}
	} else {
		if (directionChanged) return;
		directionChanged = true;
		if (key === upKey && press) {
			movePlayer(0, -1);
		} else if (key === downKey && press) {
			movePlayer(0, 1);
		} else if (key === leftKey && press) {
			movePlayer(-1, 0);
		} else if (key === rightKey && press) {
			movePlayer(1, 0);
		} else {
			directionChanged = false;
		}
	}
});