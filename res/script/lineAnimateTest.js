draw.clearColor(0x101010);

var lineHeight = 0.75,
	linePadding = 7,
	lineCount = 100,
	maxLineWidth = 100,
	imageHeight = (lineHeight + linePadding) * lineCount;

var t = 0;

event.draw = function lineAnimateTest_draw(e) {
	var topX = (sys.screenWidth / 2) - (maxLineWidth / 2),
		topY = (sys.screenHeight / 2) - (imageHeight / 2);
	for (var i = 0; i < lineCount; i++) {
		var lineWidth = Math.sin(i * t) * 500;
		draw.setColor(draw.getRGBFromHSV(i, 1, 1));
		draw.rect(i % 2 ? topX : (topX + (maxLineWidth - lineWidth)),
			topY + ((lineHeight + linePadding) * i),
			lineWidth,
			lineHeight);
	}
	t += Math.abs(sys.deltaTime / 100);
};