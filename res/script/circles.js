var circles = [];

function* rainbowGradient() {
	while (true) {
		for (var x = 0; x < 360; x += 10) {
			yield draw.getRGBFromHSV(x, 1.0, 0.5);
		}
	}
}

var rainbow = rainbowGradient();

for (var i = 0; i < 100; i++) {
	circles.push([0, rainbow.next().value]);
}

event.draw = function circles_draw(e) {
	for (var i = 0; i < circles.length; i++) {
		draw.setColor(circles[i][1]);
		draw.circle(sys.screenWidth / 2, sys.screenHeight / 2, i * 2, (i - 1) * 2, i * 2, circles[i][0], circles[i][0] + 0.5, true);
		if (i % 2 === 0) {
			circles[i][0] += Math.tan(i * 0.01) * sys.deltaTime;
		} else {
			circles[i][0] -= Math.tan(i * 0.01) * sys.deltaTime;
		}
		if (circles[i][0] > 1.0) {
			while (circles[i][0] > 1.0) {
				circles[i][0] -= 1.0;
			}
		}
		if (circles[i][0] < -1.0) {
			while (circles[i][0] < -1.0) {
				circles[i][0] += 1.0;
			}
		}
	}
};