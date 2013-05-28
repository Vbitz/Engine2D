var tex = tex ? tex : 0;

var max_iterations = 20;

function mandlebrot(rX, rY, x0, y0) {
	x0 = x0 / rX;
	y0 = y0 / rY;

	x = 0;
	y = 0;

	iteration = 0;

	while (x*x + y*y < 2*2 && iteration < max_iterations)
	{
		xtemp = x*x - y*y + x0;
		y = 2*x*y + y0;

		x = xtemp;

		iteration = iteration + 1;
	}

	//console.log(x + " : " + y);

	color = iteration;

	return {x: x, y: y, color: color};
}

var cameraX = 0;
var cameraY = 0;

sys.drawFunc(function () {
	var ratioX = 1000;
	var ratioY = 1000;
	draw.cameraPan(cameraX, cameraY);
	for (var x = 0; x < sys.screenWidth * 5; x++) {
		for (var y = 0; y < sys.screenHeight * 5; y++) {
			var val = mandlebrot(ratioX, ratioY, x, y);
			val.x *= ratioX;
			val.y *= ratioY;
			draw.setColorF(val.color / max_iterations, val.color / max_iterations, val.color / max_iterations);
			draw.rect(x * 8, y * 8, 8, 8);
		}
	}
	if (input.keyDown("W")) {
		cameraY += 5;
	}
	if (input.keyDown("S")) {
		cameraY -= 5;
	}
	if (input.keyDown("A")) {
		cameraX += 5;
	}
	if (input.keyDown("D")) {
		cameraX -= 5;
	}
});