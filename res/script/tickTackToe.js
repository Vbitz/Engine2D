// Started at Fri 4 Oct 10:29:00
// Finished at Fri 4 Oct 11:03:34
// Done in 34 minutes

var TILE_SIZE = 64,
	TILE_PADDING = 16,
	BOARD_OFFSET = 32,
	BOARD_SIZE = BOARD_OFFSET + (TILE_SIZE + TILE_PADDING) * 3,
	TEXT_OFFSET = 10,
	CURSOR_SIZE = 8;

var lastMouse = false,
	currentTurn = 1,
	win = false;

var tickTackToeBoard = [];

var colorPalate = {
	"background": 0xeaeaea,
	"text": 0x101010,
	null: 0x808080,
	2: 0xea1010,
	1: 0x1010ea
};

function resetBoard() {
	tickTackToeBoard = [
		[null, null, null],
		[null, null, null],
		[null, null, null]
	];
}

resetBoard();

function resetGame() {
	sys.forceReload("script/tickTackToe.js");
}

function checkLine(x1, y1, x2, y2, x3, y3) {
	return (tickTackToeBoard[x1][y1] === 1 && tickTackToeBoard[x2][y2] === 1 && tickTackToeBoard[x3][y3] === 1) ||
		(tickTackToeBoard[x1][y1] === 2 && tickTackToeBoard[x2][y2] === 2 && tickTackToeBoard[x3][y3] === 2);
}

function checkWin() {
	return 	checkLine(0, 0, 1, 0, 2, 0) ||
			checkLine(0, 0, 0, 1, 0, 2) ||
			checkLine(0, 1, 1, 1, 2, 1) ||
			checkLine(1, 0, 1, 1, 1, 2) ||
			checkLine(0, 2, 1, 2, 2, 2) ||
			checkLine(2, 0, 2, 1, 2, 2) ||
			checkLine(0, 0, 1, 1, 2, 2) ||
			checkLine(2, 0, 1, 1, 0, 2);	
}

function processClick(tX, tY) {
	if (tickTackToeBoard[tX][tY] === null) {
		tickTackToeBoard[tX][tY] = currentTurn;
		currentTurn = currentTurn === 1 ? 2 : 1;
	}
	if (checkWin()) {
		win = true;
	}
}

function doClick(x, y) {
	if (x < BOARD_OFFSET || y < BOARD_OFFSET) return;
	if (x > BOARD_SIZE || y > BOARD_SIZE) return;
	processClick(Math.floor((x - BOARD_OFFSET) / (TILE_SIZE + TILE_PADDING)), Math.floor((y - BOARD_OFFSET) / (TILE_SIZE + TILE_PADDING)));
}

function drawCursor(x, y) {
	draw.setColor(colorPalate.text);
	draw.rect(x - CURSOR_SIZE, y, CURSOR_SIZE * 2 + 1, 1);
	draw.rect(x, y - CURSOR_SIZE, 1, CURSOR_SIZE * 2 + 1);
}

draw.clearColor(colorPalate["background"]);

sys.drawFunc(function () {
	// draw stuff
	draw.setFont("basic", 16);
	draw.setColor(colorPalate["text"]);
	if (!win) {
		draw.print(20, BOARD_SIZE, currentTurn === 1 ? "Blue Turn" : "Red Turn");
	} else {
		draw.print(20, BOARD_SIZE, "WIN!!!!!");
	}
	draw.setFont("basic", 52);
	for (var x = 0; x < 3; x++) {
		for (var y = 0; y < 3; y++) {
			draw.setColor(colorPalate[null]);
			draw.rect(x * (TILE_SIZE + TILE_PADDING) + BOARD_OFFSET,
				y * (TILE_SIZE + TILE_PADDING) + BOARD_OFFSET,
				TILE_SIZE,
				TILE_SIZE);
			draw.setColor(colorPalate[tickTackToeBoard[x][y]]);
			draw.print(x * (TILE_SIZE + TILE_PADDING) + BOARD_OFFSET + TEXT_OFFSET,
				y * (TILE_SIZE + TILE_PADDING) + BOARD_OFFSET + TEXT_OFFSET,
				tickTackToeBoard[x][y] !== null ? (tickTackToeBoard[x][y] !== 1 ? "X" : "O") : "");
		}
	}
	drawCursor(input.mouseX, input.mouseY);
	// check mouse state
	if (input.leftMouseButton && !lastMouse) {
		if (win) {
			resetGame();
		} else {
			doClick(input.mouseX, input.mouseY);	
		}
		lastMouse = true;
	} else if (!input.leftMouseButton && lastMouse) {
		lastMouse = false;
	}
});

sys.keyboardFunc(function (a, k, p) {
	if (k === "R" && p) {
		resetGame();
	}
});