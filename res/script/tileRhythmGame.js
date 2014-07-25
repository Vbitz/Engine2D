var pattern = [];
var currentPos = 1;

function generate() {
	pattern = [];
	for (var i = 0; i < 10000; i++) {
		pattern.push(Math.floor(Math.random() * 4));
	}
} generate();

function submitKey(i) {
	if (pattern[pattern.length - currentPos] === i) {
		currentPos++;
	}
}

event.draw = function tileRG_draw (e) {
	var tileHeight = sys.screenHeight / 5;
	var tileWidth = sys.screenWidth / 4;
	draw.drawColor = "blue";
	draw.rect(pattern[pattern.length - currentPos] * tileWidth, 0, tileWidth, tileHeight);
	draw.drawColor = "white";
	draw.setFont("basic", 32);
	draw.print(tileWidth * 0 + (tileWidth / 2 - 10), 40, "A");
	draw.print(tileWidth * 1 + (tileWidth / 2 - 10), 40, "S");
	draw.print(tileWidth * 2 + (tileWidth / 2 - 10), 40, "D");
	draw.print(tileWidth * 3 + (tileWidth / 2 - 10), 40, "F");
	for (var i = 1; i < 5; i++) {
		draw.line(0, tileHeight * i, sys.screenWidth, tileHeight * i);
		draw.rect(pattern[pattern.length - currentPos - i] * tileWidth, tileHeight * i, tileWidth, tileHeight);
	}
};

event.key_A = function tileRG_A (e) {
	if (e.state === "press") submitKey(0);
};

event.key_S = function tileRG_S (e) {
	if (e.state === "press") submitKey(1);
};

event.key_D = function tileRG_D (e) {
	if (e.state === "press") submitKey(2);
};

event.key_F = function tileRG_F (e) {
	if (e.state === "press") submitKey(3);
};