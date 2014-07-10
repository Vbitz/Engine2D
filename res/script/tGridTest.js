var level = {};

function distince(p1, p2) {
	return Math.sqrt(Math.pow(p1.x - p2.x, 2) + Math.pow(p1.y - p2.y, 2));
}

function uuid() {
	return Math.random().toString(16).substring(2);
}

function generate() {
	level = {};
	var rand = new Math.Random();
	for (var i = 0; i < 500; i++) {
		level[uuid()] = {
			x: rand.nextNormal(0, 100) + (sys.screenWidth / 2),
			y: rand.nextNormal(0, 100) + (sys.screenHeight / 2),
			connections: []
		};
	}
	// Build a mesh from the items
	for (var p1 in level) {
		var smallestDistince = 10000;
		var smallestDistincePoint = null;
		for (var p2 in level) {
			var dist = distince(level[p1], level[p2]);
			if (dist < smallestDistince && p1 !== p2) {
				smallestDistince = dist;
				smallestDistincePoint = p2;
			}
		}
		if (smallestDistincePoint !== null) {
			level[p1].connections.push(smallestDistincePoint);
			level[smallestDistincePoint].connections.push(p1);
		}
	}
}

generate();

event.draw = function tGridTest_draw (e) {
	for (var i in level) {
		draw.rect(level[i].x - 2, level[i].y - 2, 4, 4);
	}
	for (var i in level) {
		var point = level[i];
		point.connections.forEach(function (nextPoint) {
			draw.line(point.x, point.y, level[nextPoint].x, level[nextPoint].y);
		});
	}
};

event.key_R = function tGridTest_regen (e) {
	if (e.state === "press") {
		generate();
	}
};