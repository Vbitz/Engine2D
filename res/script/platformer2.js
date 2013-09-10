var colors = {
	"background": 0x66666f,
	"black": 0x101010,
	"grey1": 0x303030,
	"grey2": 0x707070,
	"grey3": 0x909090,
	"white": 0xeaeaea,
	"blue1": 0x2020dd,
	"blue2": 0x4c4cf0,
	"blue3": 0x5c5cff,
	"red1": 0xdd2020,
	"red2": 0xf04c4c,
	"red3": 0xff5c5c,
	"green1": 0x20dd20,
	"green2": 0x4cf04c,
	"green3": 0x5cff5c
};

draw.clearColor(colors["background"]);

draw.setFont("basic", 12);

function shadowText(x, y, text) {
	draw.setColor(colors["grey1"]);
	draw.print(x + 1, y + 1, text);
	draw.setColor(colors["white"]);
	draw.print(x, y, text);
}

var playerX = 0;
var playerY = 0;

sys.drawFunc(function () {
	/* // colors test
	var y = 20;
	for (var i in colors) {
		draw.setColor(colors[i]);
		draw.rect(0, y, sys.screenWidth, 20);
		draw.setColor(colors["black"]);
		draw.print(11, y + 6, i);
		draw.setColor(colors["white"]);
		draw.print(10, y + 5, i);
		y += 20;
	}

	draw.grad(0, y += 20, sys.screenWidth, 20, colors["blue1"], colors["blue3"], false);
	draw.grad(0, y += 20, sys.screenWidth, 20, colors["red1"], colors["red3"], false);
	draw.grad(0, y += 20, sys.screenWidth, 20, colors["green1"], colors["green3"], false);
	*/

	shadowText(10, 25, "Platformer v2.0");

	
});