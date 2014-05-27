var points = [];
var connections = [];

var rand = new Math.Random(5);

for (var i = 0; i < 100; i++) {
	points.push({x: rand.next(sys.screenWidth), y: rand.next(sys.screenHeight)});
}

points.forEach(function (p) {
	points.forEach(function (p2) {
		var dist = Math.sqrt(Math.pow(p.x - p2.x, 2) + Math.pow(p.y - p2.y, 2));
		if (dist < 150) {
			connections.push({
				col: draw.getRGBFromHSV(0.0, 0.1, dist / 100),
				x1: p.x,
				y1: p.y,
				x2: p2.x,
				y2: p2.y
			});
		}
	});
});

event.draw = function grid_draw(e) {
	connections.forEach(function (c) {
		draw.setColor(c.col);
		draw.line(c.x1, c.y1, c.x2, c.y2);
	});

	points.forEach(function (p) {
		var dist = Math.sqrt(Math.pow(p.x - input.mouseX, 2) + Math.pow(p.y - input.mouseY, 2));
		if (dist < 360) {
			draw.setColor(draw.getRGBFromHSV(dist, 1.0, 1.0));
			draw.line(p.x, p.y, input.mouseX, input.mouseY);
		}
	});

	draw.setColor(0xeaeaea);

	points.forEach(function (p) {
		draw.rect(p.x - 5, p.y - 5, 10, 10);
	});
};