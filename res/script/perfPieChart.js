var pieColors = ["blue", "red", "green", "yellow", "pink"];

function perfPieChart(x, y, radius) {
	var textStartX = x + radius + 32;
	var textStartY = y;
	draw.setFont("basic", 10);

	var perfZones = sys.getProfilerZones(true);

	var perfFullTotal = sys.getProfilerTime("Draw");
	var perfTotal = 0;
	var currentColor = 0;

	perfZones.forEach(function (zone) {
		if (zone.indexOf("Root::Frame::Draw") !== 0) return;
		var zoneTokens = zone.split("::");
		if (zoneTokens.length !== 4) return;
		var zoneTime = sys.getProfilerTime(zoneTokens[zoneTokens.length - 1]);
		draw.setColor(pieColors[currentColor++]);
		draw.circle(x, y, radius, radius / 2, perfTotal, perfTotal + (zoneTime / perfFullTotal), true);
		perfTotal += (zoneTime / perfFullTotal);
		draw.print(textStartX, textStartY, zone + " : " + zoneTime.toFixed(8));
		textStartY += 16;
	});
}

sys.on("draw", "perfPieChart.draw", function () {
	perfPieChart(256, 256, 128);
});