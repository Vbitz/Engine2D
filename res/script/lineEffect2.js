var time = 0;
var x, y;

event.draw = function lineEffect2_draw() {
	time += sys.deltaTime;
	var sinTime = Math.sin(time);
	for (x = 0; x < 100; x++) {
		draw.setColor(draw.getRGBFromHSV(Math.cos(x) * 90 + 180, 1, 1));
		draw.rect((x * 5) + 300 + (sinTime * 100), 300, 1, x * sinTime + x * 5);
	}
	for (y = 0; y < 100; y++) {
		draw.setColor(draw.getRGBFromHSV(Math.sin(y) * 90, 1, 1));
		draw.rect(300, (y * 5) + 300 + (sinTime * 100), y * sinTime + y * 5, 1);
	}
};
