draw.clearColor(0xeaeaea);

var currentX = 100;
var currentY = 100;

var lastMoves = [];

var initalWormSize = 8;
var currentHealth = 100;
var wormSize = initalWormSize;

var worldRects = [];

function rand(min, max) {
	if (max === undefined) {
		return Math.floor(Math.random() * min);
	} else {
		return Math.floor(Math.random() * (max - min)) + min;
	}
}

function makeRect(x, y, w, h, color, col) {
	return {
		x: x,
		y: y,
		width: w,
		height: h,
		active: true,
		color: (color === undefined ? 0xbbccdd : color),
		onCol: (col === undefined ? function () { return false; } : col)
	};
}

function addToWorld(r) {
	worldRects.push(r);
}

function getPlayerRect() {
	return makeRect(currentX - 10, currentY - 10, 20, 20);
}

function drawRect(r) {
	draw.setColor(r.color);
	draw.rect(r.x, r.y, r.width, r.height);
}

// modifyed from http://stackoverflow.com/questions/2752349/fast-rectangle-to-rectangle-intersection
function intersectRect(r1, r2) {
  return !(r2.x > r1.x + (r1.width) || 
           r2.x + (r2.width) < r1.x || 
           r2.y > r1.y + (r1.height) ||
           r2.y + (r2.height) < r1.y);
}

function validMove(x, y) {
	if (x < 0)					return false;
	if (x > sys.screenWidth)	return false;
	if (y < 0)					return false;
	if (y > sys.screenHeight)	return false;
	var playerRect = makeRect(x - 10, y - 10, 20, 20);
	for (var i = 0; i < worldRects.length; i++) {
		if (intersectRect(playerRect, worldRects[i]) && worldRects[i].active) {
			return worldRects[i].onCol.apply(worldRects[i], [x, y]);
		}
	}
	return true;
}

function addFood() {
	addToWorld(makeRect(Math.floor(rand(0, sys.screenWidth) / 20) * 20,
		Math.floor(rand(0, sys.screenHeight) / 20) * 20,
		20, 20, 0xffaabb,
	function () {
		wormSize += 5;
		this.active = false;
		addFood();
		currentHealth += 100;
		return false;
	}));
}

for (var i = 0; i < rand(20, 50); i++) {
	addToWorld(makeRect(Math.floor(rand(0, sys.screenWidth) / 20) * 20,
		Math.floor(rand(0, sys.screenHeight) / 20) * 20,
		20, 20));
}

addFood();

sys.drawFunc(function () {
	draw.setColor(0xa0b0c0);
	lastMoves.forEach(function (i) {
		draw.circle(i.x, i.y, 10, 0, 6, 0, 1.0, true);
	});
	draw.setColor(0x708090);
	draw.circle(currentX, currentY, 15, 0, 6, 0, 1.0, true);

	worldRects.forEach(function (i) {
		if (i.active) {
			drawRect(i);
		}
	});

	draw.setColor(0x101010);
	draw.print(10, 20, "Points: " + ((wormSize - initalWormSize) * 100));
	draw.print(10, 35, "Health: ");
	draw.setColor(0xffaabb);
	draw.rect(60, 35, (currentHealth / 1000) * (sys.screenWidth - 200), 10);
});

sys.keyboardFunc(function (a, chr, press) {
	if (!press) {
		return;
	}
	if (chr === "W" && validMove(currentX, currentY - 10)) {
		currentY -= 10;
	} else if (chr === "S" && validMove(currentX, currentY + 10)) {
		currentY += 10;
	} else if (chr === "A" && validMove(currentX - 10, currentY)) {
		currentX -= 10;
	} else if (chr === "D" && validMove(currentX + 10, currentY)) {
		currentX += 10;
	} else {
		return;
	}
	lastMoves.push({
		x: currentX,
		y: currentY
	});
	currentHealth -= ((sys.screenWidth + sys.screenHeight) / 2) * 0.000015 * wormSize;
	
	if (currentHealth < 0) {
		draw.clearColor(0x101010);
		sys.drawFunc(function () {
			draw.setColor(0xff7777);
			draw.setFont("basic", 36);
			draw.print(100, 100, "You DIED!!!");
		});
		sys.keyboardFunc(function () {

		});
	} else if (currentHealth > 1000) {
		draw.clearColor(0x101010);
		sys.drawFunc(function () {
			draw.setColor(0x77ff77);
			draw.setFont("basic", 36);
			draw.print(100, 100, "You WIN!!!");
		});
		sys.keyboardFunc(function () {

		});
	}

	if (lastMoves.length > wormSize) {
		lastMoves = lastMoves.splice(1);
	}
});