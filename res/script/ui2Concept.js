var windowBackgroundColor = 0x101010,
	windowTitlebarColor = 0x181818,
	windowBorderColor = 0x404040,
	windowTitleColor = 0xb0b0b0;

var searchBoxBorderColor = 0x606060,
	searchBoxBackgroundColor = 0x202020,
	searchBoxTextColor = 0xb0b0b0;

function borderedRectangle(x, y, w, h, borderColor, fillColor) {
	draw.setColor(borderColor);
	draw.grid(x, y, w, h);
	draw.setColor(fillColor);
	draw.rect(x, y, w, h);
}

sys.on("draw", "ui2Concept.draw", function () {
	// window 1, basic search
	borderedRectangle(100, 100, 200, 360, windowBorderColor, windowBackgroundColor);

	// titlebar
	borderedRectangle(100, 100, 200, 14, windowBorderColor, windowTitlebarColor);

	// titlebar text
	draw.setFont("basic", 11);
	draw.setColor(windowTitleColor);
	draw.print(102, 102, "Hello World");

	// search box
	borderedRectangle(105, 120, 190, 14, searchBoxBorderColor, searchBoxBackgroundColor);

	// search box text
	draw.setFont("basic", 12);
	draw.setColor(searchBoxTextColor);
	draw.print(107, 122, "Hello");
	draw.rect(107 + draw.getStringWidth("Hello") + 2, 120, 1, 12);
});