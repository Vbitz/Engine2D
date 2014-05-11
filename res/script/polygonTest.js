function drawOctogon(x, y, xScale, yScale) {
	draw.polygon(x, y, [
		0 * xScale,		-1 * yScale,
		0.75 * xScale,	-0.75 * yScale,
		1 * xScale,		0 * yScale,
		0.75 * xScale,	0.75 * yScale,
		0 * xScale,		1 * yScale,
		-0.75 * xScale,	0.75 * yScale,
		-1 * xScale,	0 * yScale,
		-0.75 * xScale,	-0.75 * yScale,
		0 * xScale,		-1 * yScale
	]);
}

event.draw = function polygonTest_draw(e) {
	draw.setColor(0xffffff);
	drawOctogon(120, 120, 40, 40);
	draw.setColor(0x101010);
	drawOctogon(120, 120, 30, 30);
	draw.setColor(0xabcdef);
	drawOctogon(120, 120, 10, 10);
};