"use strict";

function getCirclePoint(xCenter, yCenter, radius, angle) {
	return {
		x: xCenter + radius * Math.cos(angle),
		y: yCenter + radius * Math.sin(angle)
	};
}

function getCirclePoints(radius, radiusTMinus1, angle, angleTMinus1) {
	return [
		radius * Math.cos(angle),				radius * Math.sin(angle),
		radius * Math.cos(angleTMinus1),		radius * Math.sin(angleTMinus1),
		radiusTMinus1 * Math.cos(angleTMinus1),	radiusTMinus1 * Math.sin(angleTMinus1)
	];
}

var rand = new Math.Random(+(new Date()));

function genSnowColor() {
	return draw.getRGBFromHSV(rand.nextNormal(200, 0.1),
		rand.nextNormal(0.3, 0.1),
		rand.nextNormal(0.5, 0.05));
}

// flakes are orginized into layers with a randomly generated number of points tending towards a lower amount

function makeFlake() {
	var ret = [];
	var lastCircleRadius = 0;

	for (var i = 1 ; i < 6; i++) {
		ret[i] = [];
		var circleRadius = Math.abs(rand.nextNormal(i * 50, 10));
		var pointCount = ((6 - i) * 10 + 10);
		var pointStep = Math.PI * 2 / pointCount;
		var pointMod = Math.random() / 5;
		for (var p = 0; p < Math.PI * 2; p += pointStep) {
			ret[i].push({
				position: getCirclePoint(300, 300, circleRadius, p),
				points: getCirclePoints(circleRadius, lastCircleRadius, p, p - (pointStep / pointMod)),
				color: genSnowColor()
			});
		}
		lastCircleRadius = circleRadius;
	}

	return ret;
}

var flake = makeFlake();

//console.log(JSON.stringify(flake, 0, '\t'));

event.draw = function snowflake_draw(e) {
	Math.PI_2;
	flake.forEach(function (layer, index) {
		layer.forEach(function (point, index) {
			//draw.rect(point.position.x, point.position.y, 1, 1);
			draw.drawColor = point.color;
			draw.polygon(300, 300, point.points);
		});
	});
};

event.input = function snowflake_input(e) {
	//console.log(e.state, e.key);
	if (e.key == "R") flake = makeFlake();
};