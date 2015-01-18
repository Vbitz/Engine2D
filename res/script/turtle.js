var turtlePos = new Math.Vector(1000, 450);

var turtlePositions = [turtlePos];

var currentAngle = 0;

function move(units) {
	turtlePositions.push(turtlePos);
	var turnedPos = new Math.Vector(units, 0).rotate(currentAngle, new Math.Vector(0, 0, 1));
	turtlePos = turtlePos.add(turnedPos);
}

function turn(angle) {
	currentAngle += Math.degToRad(angle);
}

turn(180);
move(10);
for (var i = 0; i < 100; i++) {
	turn(i);
	move(600);
	turn(90);
	move(5);
	turn(90);
	move(5);
}

turtlePositions.push(turtlePos);

event.draw = function turtle_draw (e, dt) {
	for (var i = 0; i < turtlePositions.length - 1; i++) {
		draw.line(turtlePositions[i].x, turtlePositions[i].y, turtlePositions[i + 1].x, turtlePositions[i + 1].y);
	}
};