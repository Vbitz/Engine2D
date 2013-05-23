var levelWidth = 2200;
var levelHeight = 500;

var gravity = 0.05;

var friction = 0.002;

var player = {
	x: 60,
	y: -40,
	width: 20,
	height: 60,
	color: 0xffffff,

	xSpeed: 0,
	ySpeed: 0,
	xAccel: 0,
	yAccel: 0,

	inAir: true,

	isJumping: false,
	movingWest: false,
	movingEast: false,

	jumpTimer: 0
};

var level = [
	{x: -levelWidth, y: levelHeight - 20, width: levelWidth * 2, height: 10000, color: 0xbadcaa},
	{x: -levelWidth, y: -levelHeight, width: 100, height: 10000, color: 0xbadcaa},
	{x: levelWidth * 2, y: -levelHeight, width: 100, height: 10000, color: 0xbadcaa}
];

function rectColCheck(rect1, rect2) {
	if (rect1.x > (rect2.x + rect2.width) || (rect1.x + rect1.width) < rect2.x) { return false; }
	if (rect1.y > (rect2.y + rect2.height) || (rect1.y + rect1.height) < rect2.y) { return false; }
	return true;
}

draw.clearColor(0.2, 0.3, 0.6);

sys.drawFunc(function () {
	draw.cameraPan(sys.screenWidth / 2 - player.width - player.x, sys.screenHeight / 2 - player.height - player.y);

	if (player.inAir) {
		player.yAccel += gravity;
	} else {
		player.yAccel = 0;
	}

	if (player.xAccel > 2) {
		player.xAccel = 2;
	} else if (player.xAccel < -2) {
		player.xAccel = -2;
	}

	if (player.yAccel > 2) {
		player.yAccel = 2;
	} else if (player.yAccel < -2) {
		player.yAccel = -2;
	}

	player.xSpeed += player.xAccel;
	player.ySpeed += player.yAccel;

	if (player.xSpeed > 15) {
		player.xSpeed = 15;
	} else if (player.xSpeed < -15) {
		player.xSpeed = -15;
	}

	if (player.ySpeed > 15) {
		player.ySpeed = 15;
	} else if (player.ySpeed < -15) {
		player.ySpeed = -15;
	}

	if (player.isJumping) {
		player.yAccel -= 0.2;
		player.y -= 01;
		player.inAir = true;
		if (player.jumpTimer++ > 3) {
			player.isJumping = false;
		}
	}

	if (player.movingWest) {
		player.xAccel -= 0.03;
	}

	if (player.movingEast) {
		player.xAccel += 0.03;
	}

	if (player.xAccel === 0) {
		player.xSpeed = 0;
	}

	if (player.yAccel === 0) {
		player.ySpeed = 0;
	}

	player.x += player.xSpeed;
	player.y += player.ySpeed;

	if (player.xAccel !== 0 || player.yAccel !== 0) {
		player.inAir = true;
	}

	for (i = 0; i < level.length; i++) {
		var col = rectColCheck(level[i], player);
		if (col) {
			player.inAir = false;
			player.isJumping = false;
			player.jumpTimer = 0;
		}
		draw.setColor(level[i].color);
		draw.rect(level[i].x, level[i].y, level[i].width, level[i].height);
	}

	draw.setColor(player.color);
	draw.rect(player.x, player.y, player.width, player.height);

	draw.cameraReset();
	draw.setColor(0xdffabf);
	draw.print(10, 10, "x: " + player.x.toFixed(2));
	draw.print(10, 40, "y: " + player.y.toFixed(2));
	draw.print(10, 70, "xSpeed: " + player.xSpeed.toFixed(2));
	draw.print(10, 100, "ySpeed: " + player.ySpeed.toFixed(2));
	draw.print(10, 130, "xAccel: " + player.xAccel.toFixed(2));
	draw.print(10, 160, "yAccel: " + player.yAccel.toFixed(2));
});

sys.keyboardFunc(function (type, key, press) {
	console.log(type + " : " + key + " : " + press);
	if (key === " ") {
		player.isJumping = press;
	}
	if (key === "A") {
		player.movingWest = press;
	}
	if (key === "D") {
		player.movingEast = press;
	}
});