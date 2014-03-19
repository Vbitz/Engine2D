var baseColor = draw.getRGBFromHSV(Math.floor(Math.random() * 360), 1.0, 0.75);

draw.clearColor(baseColor);

function clamp(val, min, max) {
	if (val > max)	return max;
	if (val < min)	return min;
	else			return val;
}

function colorMul(col, value) {
	if (typeof value === "number") {
		value = {r: value, g: value, b: value};
	}
	return {r: clamp(col.r * value.r, 0.0, 1.0),
			g: clamp(col.g * value.g, 0.0, 1.0),
			b: clamp(col.b * value.b, 0.0, 1.0)};
}

var palate = [colorMul(baseColor, 0.25), colorMul(baseColor, 0.5), colorMul(baseColor, 0.75), colorMul(baseColor, 1.0),
				colorMul(baseColor, 1.25), colorMul(baseColor, 1.5)];

function drawLine(vert, pos, width) {
	if (vert) {
		draw.rect(pos, 0, width, sys.screenHeight);
	} else {
		draw.rect(0, pos, sys.screenWidth, width);
	}
}

function genPatternLines(space, width) {
	var ret = [];
	for (var i = 0; i < 2000; i += Math.random() * space) {
		ret.push([i, Math.random() * width]);
	}
	return ret;
}

var lines1 = genPatternLines(10, 20);
var lines2 = genPatternLines(15, 5);

function drawPatternLines(lines, vert) {
	lines.forEach(function (line) {
		drawLine(vert, line[0], line[1]);
	});
}

sys.on("draw", "patternTest.draw", function () {
	draw.setColor(palate[0]);
	drawPatternLines(lines1, true);
	draw.setColor(palate[1]);
	drawPatternLines(lines1, false);
	draw.setColor(palate[2]);
	drawPatternLines(lines2, true);
	draw.setColor(palate[3]);
	drawPatternLines(lines2, false);
});