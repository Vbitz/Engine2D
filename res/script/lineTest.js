sys.on("draw", "lineTest.draw", function() {
	draw.setColor("#ffffff");
	for (var x = 0; x < sys.screenWidth; x += 100) {
		for (var y = sys.screenHeight; y > 0; y -= 100) {
			draw.line(x, 0, 0, y);
		}
	}
});