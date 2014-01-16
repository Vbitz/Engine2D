sys.on("draw", "", function () {
	draw.setColor({r: 0.43, g: 0.11, b: 0.1123});
	draw.line(0, 0, 100, 100);
	draw.line(100, 0, 0, 100);
	draw.line(0, 100, 0, 100);
	draw.line(100, 0, 100, 0);
	draw.line(0, 100, 100, 0);
});