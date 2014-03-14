/* jshint esnext: true */

var counter = 0;
var circleColor = 0x101010;

function* genGradient() {
	while (true) {
		for (var x = 0; x < 360; x++) {
			yield draw.getRGBFromHSV(x, 1.0, 0.5);
		}
	}
}

var generator = genGradient();

setInterval(function () {
	circleColor = generator.next().value;
}, 100);

sys.on("draw", "helloWorld2.draw", function () {
	draw.setColor(circleColor);
	draw.rect(Math.sin(counter) * 100 + 150,
		Math.sin(counter) * 100 + 150, 100, 100);
	counter += sys.deltaTime + (Math.random() / 16);
});