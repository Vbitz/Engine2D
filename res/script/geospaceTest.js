/* jshint esnext: true */

"use strict";

var focusedPoints = [];

function distince(x1, y1, x2, y2) {
	return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
}

function hashPoint(x, y) {
	return x + "-" + y;
}

var inc = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9];

function PointMap() {
	let self = this;
	this.blocks = new Map();

	inc.forEach(function (x) {
		inc.forEach(function (y) {
			self.blocks.set(hashPoint(x, y), []);
		});
	});
}

PointMap.prototype.addPoint = function (x, y) {
	let xBlock = (Math.floor(x * 10) / 10);
	let yBlock = (Math.floor(y * 10) / 10);
	this.blocks.get(hashPoint(xBlock, yBlock)).push({x: x, y: y});
};

PointMap.prototype.draw = function (w, h) {
	this.blocks.values().forEach(function (value, key) {
		value.forEach(function (point) {
			draw.rect(point.x, point.y, 1, 1);
		});
	});
};

PointMap.prototype.findByRadius = function (x, y, r) {
	var ret = [];
	for (let item of this.blocks.values()) {
		ret = ret.concat(item.filter(function (i) {
			return distince(x, y, i.x, i.y) < r;
		}));
	}
	return ret;
};

PointMap.prototype.findByBox = function (x1, y1, x2, y2) {

};

var m = new PointMap();

for (var i = 0; i < 20000; i++) {
	m.addPoint(Math.random(), Math.random());
}

event.draw = function geospaceTest_draw(e) {
	draw.drawColor = "white";
	m.draw(sys.screenWidth, sys.screenHeight);
	draw.drawColor = "red";
	this.focusedPoints.forEach(function (p) {
		draw.rect(p.x * sys.screenWidth, p.y * sys.screenHeight, 2, 2);
	});
};

event.mouseButton = function geospaceTest_mouseButton(e) {
	if (e.button !== "mouseLeft") return;
	//console.log(e.x, e.y);
	focusedPoints = m.findByRadius(e.x / sys.screenWidth, e.y / sys.screenHeight, 0.05);
	//console.log("found " + focusedPoints.length + " points");
};