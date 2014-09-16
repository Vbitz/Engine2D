// C++ Version of Pong
// This will not compile but it's a exersize in seeing how easy it is to port between JS and C++

#include "../Application.hpp"
#include "../Draw2D.hpp"
#include "../Events.hpp"
#include "../FramePerfMonitor.hpp"

#include <math.h>
#include <string>
#include <sstream>

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

	Draw2DPtr draw2D = NULL;

	void respawnBall(ApplicationPtr app, bool player) {
		glm::vec2 size = app->GetWindow()->GetWindowSize();
		
		ballX = (size.x / 2) - (ballSize / 2);
		ballY = (size.y / 2) - (ballSize / 2);

		ballAccelX = (randF() * 400) - 150 * 1.5;
		ballAccelY = (player) ? 400 : -400; // start on a random side
	}

	EventMagic pong_draw(Json::Value args, void* userPointer) {
		ApplicationPtr app = (ApplicationPtr) userPointer;

		glm::vec2 size = app->GetWindow()->GetWindowSize();

		draw2D->GetRender()->SetColor(0x0aaaaa);
		draw2D->Grid(0, 64, size.x, size.y - 128);

		// draw ball
		draw2D->Circle(ballX, ballY, 10, true);

		// draw player 1 
		draw2D->Rect(player1PaddleX - (paddleWidth / 2), size.y - 64 - 8, paddleWidth, 16);
		
		// draw player 2
		draw2D->Rect(player2PaddleX - (paddleWidth / 2), 64 - 8, paddleWidth, 16);

		std::stringstream ss;

		ss << "Score: " << player1Score << "-" << player2Score;

		// draw score
		draw2D->GetRender()->SetFont("basic", 24);
		draw2D->GetRender()->SetColor(0x006060);
		draw2D->GetRender()->Print(20, size.y / 2 - 12, ss.str().c_str());
		
		// update game state
		float dt = FramePerfMonitor::GetFrameTime();

		// update player 1 position
		if 		(app->GetKeyPressed('A') && player1PaddleX > 0)			{ player1PaddleX -= player1Speed * dt; }
		else if (app->GetKeyPressed('D') && player1PaddleX < size.x)	{ player1PaddleX += player1Speed * dt; }

		// update player 2 position based on player 1
		player2PaddleX = lerp(player2PaddleX, ballX, dt * 8);

		// update the ball and check for colisions
		ballX += ballAccelX * dt;
		ballY += ballAccelY * dt;

		if (ballX < 0 || ballX > size.x) {
			ballAccelX = -ballAccelX;
		}

		if (ballY < 64) {
			if (ballX > player2PaddleX - (paddleWidth / 2) && ballX < player2PaddleX + (paddleWidth / 2)) {
				ballAccelY = -ballAccelY;
				ballAccelX = ballAccelX * 1.1;
			} else {
				player1Score++;
				respawnBall(app, true);
			}
		} else if (ballY > size.y - 64) {
			if (ballX > player1PaddleX - (paddleWidth / 2) && ballX < player1PaddleX + (paddleWidth / 2)) {
				ballAccelY = -ballAccelY;
				ballAccelX = ballAccelX * 1.1;
			} else {
				player2Score++;
				respawnBall(app, false);
			}
		}

		return EM_OK;
	}

	void startup() {
		ApplicationPtr app = GetAppSingilton();

		draw2D = new Draw2D(app->GetRender());

		glm::vec2 size = app->GetWindow()->GetWindowSize();

		player1Speed = size.x;

		player1PaddleX = (size.x / 2);
		player1PaddleX = (size.x / 2);

		respawnBall(app, randF() > 0.5f);
		GetEventsSingilton()->GetEvent("draw")->AddListener("pongTest.draw", EventEmitter::MakeTarget(pong_draw, app));
	}

}

ENGINE_ADDON("pongTest", "Vbitz", PongTest::startup, NULL, LoadOrder::PreScript)