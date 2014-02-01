// Started at Sat 1st Feb 2014 1:25pm
// Everything but ball physics done at 1:52pm
// Ball Physics done without polish at 1:57pm
// Basic Version done at 2:09pm (44 minutes)

function lerp(v0, v1, t) {
  return v0+(v1-v0)*t;
}

var paddleWidth = 196,
	ballSize = 32;

var player1Speed = sys.screenWidth;

// Player 1 is player controled and Player 2 is computer controled with a lerp function based AI
var player1PaddleX = (sys.screenWidth / 2),
	player2PaddleX = (sys.screenWidth / 2);

var player1Score = 0,
	player2Score = 0;

var ballX, ballY, ballAccelX, ballAccelY;

function respawnBall(player) {
	ballX = (sys.screenWidth / 2) - (ballSize / 2);
	ballY = (sys.screenHeight / 2) - (ballSize / 2);

	ballAccelX = (Math.random() * 400) - 150 * 1.5;
	ballAccelY = (player) ? 400 : -400; // start on a random side
}

respawnBall(Math.random() > 0.5);

sys.clearEvent("pong.draw");
sys.on("draw", "pong.draw", function () {
	draw.setColor(0x0aaaaa);
	draw.grid(0, 64, sys.screenWidth, sys.screenHeight - 128);

	// draw ball
	draw.circle(ballX, ballY, 10, true);

	// draw player 1 
	draw.rect(player1PaddleX - (paddleWidth / 2), sys.screenHeight - 64 - 8, paddleWidth, 16);
	
	// draw player 2
	draw.rect(player2PaddleX - (paddleWidth / 2), 64 - 8, paddleWidth, 16);

	// draw score
	draw.setFont("basic", 24);
	draw.setColor(0x006060);
	draw.print(20, sys.screenHeight / 2 - 12, "Score: " + player1Score + "-" + player2Score);
	
	// update game state
	var dt = sys.deltaTime;

	// update player 1 position
	if 		(input.keyDown("A") && player1PaddleX > 0)					{ player1PaddleX -= player1Speed * dt; }
	else if (input.keyDown("D") && player1PaddleX < sys.screenWidth)	{ player1PaddleX += player1Speed * dt; }

	// update player 2 position based on player 1
	player2PaddleX = lerp(player2PaddleX, ballX, dt * 8);

	// update the ball and check for colisions
	ballX += ballAccelX * dt;
	ballY += ballAccelY * dt;

	if (ballX < 0 || ballX > sys.screenWidth) {
		ballAccelX = -ballAccelX;
	}

	if (ballY < 64) {
		if (ballX > player2PaddleX - (paddleWidth / 2) && ballX < player2PaddleX + (paddleWidth / 2)) {
			ballAccelY = -ballAccelY;
			ballAccelX = ballAccelX * 1.1;
		} else {
			player1Score++;
			respawnBall(true);
		}
	} else if (ballY > sys.screenHeight - 64) {
		if (ballX > player1PaddleX - (paddleWidth / 2) && ballX < player1PaddleX + (paddleWidth / 2)) {
			ballAccelY = -ballAccelY;
			ballAccelX = ballAccelX * 1.1;
		} else {
			player2Score++;
			respawnBall(false);
		}
	}
});