var resultPools = [];

var rand = new Math.Random();

var maxValue = 0;
var maxXValue = -1;

function addNum(num) {
	num = Math.abs(num);
	num = Math.floor(num * 4);
	if (resultPools[num] === undefined) {
		resultPools[num] = 0;
	}
	resultPools[num]++;
	if (resultPools[num] > maxValue) {
		maxValue = resultPools[num];
	}
	if (num > maxXValue) {
		maxXValue = num;
	}
}

setInterval(function () {
	addNum(rand.nextNormal(0, 5));
}, 2);

sys.on("draw", "normalRandomTest.draw", function (e) {
	var scale = sys.screenHeight / maxValue;
	var hsvScale = 360 / maxXValue;
	var hsvValue = 0;
	for (var i = 0; i < resultPools.length; i++) {
		var value = resultPools[i];
		draw.setColor(draw.getRGBFromHSV(hsvValue, 0.8, 0.5));
		hsvValue += hsvScale;
		draw.rect(i * 20, sys.screenHeight + 30 - (value * scale), 15, value * scale - 40);
	}
	draw.print(20, 20, "maxValue=" + maxValue + " : scale=" + scale);
});