// C++ Version of Pong
// This will not compile but it's a exersize in seeing how easy it is to port between JS and C++

#include "Application.hpp"
#include "Draw2D.hpp"
#include "Events.hpp"
#include "FramePerfMonitor.hpp"

#include <math.h>
#include <string>
#include <stringstream>

using namespace Engine;

namespace PongTest {

	float randF() {
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	float lerp(float v0, float v1, float t) {
	  return v0+(v1-v0)*t;
	}

	int paddleWidth = 196;
	int ballSize = 32;

	float player1Speed = 0.0f;

	// Player 1 is player controled and Player 2 is computer controled with a lerp function based AI
	float player1PaddleX = 0.0f;
	float player2PaddleX = 0.0f;

	int player1Score = 0;
	int player2Score = 0;

	float ballX, ballY, ballAccelX, ballAccelY;

	void respawnBall(bool player) {
		Application* app = GetAppSingilton();
		
		ballX = (app->GetScreenWidth() / 2) - (ballSize / 2);
		ballY = (app->GetScreenHeight() / 2) - (ballSize / 2);

		ballAccelX = (randF() * 400) - 150 * 1.5;
		ballAccelY = (player) ? 400 : -400; // start on a random side
	}

	void pong_draw(Events::EventArgs e) {
		Application* app = GetAppSingilton();

		Draw2D::SetColor(0x0aaaaa);
		Draw2D::Grid(0, 64, sys.screenWidth, sys.screenHeight - 128);

		// draw ball
		Draw2D::Circle(ballX, ballY, 10, true);

		// draw player 1 
		Draw2D::Rect(player1PaddleX - (paddleWidth / 2), sys.screenHeight - 64 - 8, paddleWidth, 16);
		
		// draw player 2
		Draw2D::Rect(player2PaddleX - (paddleWidth / 2), 64 - 8, paddleWidth, 16);

		std::stringstream ss;

		ss << "Score: " << player1Score << "-" << player2Score;

		// draw score
		Draw2D::SetFont("basic", 24);
		Draw2D::SetColor(0x006060);
		Draw2D::Print(20, app->GetScreenHeight() / 2 - 12, ss.str().c_str());
		
		// update game state
		float dt = FramePerfMonitor::GetFrameTime();

		// update player 1 position
		if 		(app->GetKeyPressed('A') && player1PaddleX > 0)					{ player1PaddleX -= player1Speed * dt; }
		else if (app->GetKeyPressed('D') && player1PaddleX < sys.screenWidth)	{ player1PaddleX += player1Speed * dt; }

		// update player 2 position based on player 1
		player2PaddleX = lerp(player2PaddleX, ballX, dt * 8);

		// update the ball and check for colisions
		ballX += ballAccelX * dt;
		ballY += ballAccelY * dt;

		if (ballX < 0 || ballX > app->GetScreenWidth()) {
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
		} else if (ballY > app->GetScreenHeight() - 64) {
			if (ballX > player1PaddleX - (paddleWidth / 2) && ballX < player1PaddleX + (paddleWidth / 2)) {
				ballAccelY = -ballAccelY;
				ballAccelX = ballAccelX * 1.1;
			} else {
				player2Score++;
				respawnBall(false);
			}
		}
	}

	void init() {
		Application* app = GetAppSingilton();

		player1Speed = app->GetScreenWidth();

		player1PaddleX = (app->GetScreenWidth() / 2);
		player1PaddleX = (app->GetScreenWidth() / 2);

		respawnBall(randF() > 0.5f);

		Events::On("draw", "pong.draw", pong_draw);
	}

}