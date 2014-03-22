var background = 0x87CEEB,
	floorColor = 0x106410,
	playerColor = 0x101010,
	obsticle1Color = 0x30a030,
	obsticle2Color = 0x30a030;

var floorHeight = 60;

var cameraX = -300,
	cameraSpeed = 80;

var playerY = sys.screenHeight / 2;

// speed mesured in pixels/second
var playerSpeedY = 0;

var playerAccelerationY = 0;

var obsticles = []; // obsticles in the format of {y: y, height: height}
					// V8 should create a hidden class to store this effectively
var lastObsticlePos = 100,
	obsticleDistince = 300;

draw.clearColor(background);

function lose() {

}

function rand(min, max) {
	if (max === undefined) {
		max = min;
		min = 1.0;
	}
	return Math.floor(Math.random() * (max - min)) + min; 
}

function addObsticle() {
	obsticles.push({
		x: lastObsticlePos,
		height: rand(sys.screenHeight / 5, sys.screenHeight / 5 * 3)
	});
	lastObsticlePos += obsticleDistince;
	if (obsticles.length > 10) {
		obsticles = obsticles.slice(1);
	}
}

addObsticle();

sys.clearEvent("flapFlap.draw");
sys.on("draw", "flapFlap.draw", function (e) {
	// draw player
	draw.setColor(playerColor);
	draw.circle(100, playerY, 20, true);

	draw.cameraPan(-cameraX, 0);

	// draw obsticles
	draw.setColor(obsticle1Color);
	obsticles.forEach(function (o) {
		draw.rect(o.x - 40, 0, 80, sys.screenHeight);
	});

	// draw floor
	draw.cameraReset();
	draw.setColor(floorColor);
	draw.rect(0, sys.screenHeight - floorHeight, sys.screenWidth, floorHeight);
	
	draw.setColor(0x101010);
	draw.print(32, 32, "Accel: " + playerAccelerationY.toString(10));
	draw.print(32, 44, "Speed: " + playerSpeedY.toString(10));
	
	// update player
	var dt = sys.deltaTime;

	cameraX += cameraSpeed * dt;
	if (cameraX > (lastObsticlePos - sys.screenWidth - 200)) {
		addObsticle();
	}

	playerY += playerSpeedY * dt;
	playerSpeedY += playerAccelerationY * dt;

	playerAccelerationY += playerAccelerationY < 0 ? (-playerAccelerationY * dt + 1) : 0.1;
	if (playerAccelerationY > 20.0) {
		playerAccelerationY = 20.0;
	}
});

sys.clearEvent("flapFlap.input");
sys.on("input", "flapFlap.input", function (e) {
	if (e.key === ' ' && e.state === "press" && playerAccelerationY > 0) {
		playerAccelerationY -= 80;
	}
});