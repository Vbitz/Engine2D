var circles = [];

var ringSize = 2.0;

function* rainbowGradient() {
	while (true) {
		for (var x = 0; x < 360; x += 2) {
			yield draw.getRGBFromHSV(x, 1.0, 0.5);
		}
	}
}

var rainbow = rainbowGradient();

for (var i = 0; i < 400; i++) {
	circles.push([0, rainbow.next().value]);
}

event.draw = function circles_draw(e) {
	for (var i = 0; i < circles.length; i++) {
		draw.setColor(circles[i][1]);
		draw.circle(sys.screenWidth / 2, sys.screenHeight / 2, i * ringSize, (i - 1) * ringSize, 3, circles[i][0], Math.abs(circles[i][0] + Math.sin(i) * 0.1), true);
		draw.circle(sys.screenWidth / 2, sys.screenHeight / 2, i * ringSize, (i - 1) * ringSize, 3, circles[i][0] + 0.5, Math.abs(circles[i][0] + (Math.sin(i) * 0.1)) + 0.5, true);
		if (i % 2 === 0) {
			circles[i][0] += Math.sin(i * 0.01) * sys.deltaTime * 0.05;
		} else {
			circles[i][0] -= Math.sin(i * 0.01) * sys.deltaTime * 0.05;
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