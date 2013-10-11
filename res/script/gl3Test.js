draw.setFont("basic", 16);

draw.clearColor(0xffffff);

var img = img ? img : draw.openImage("image/testing.png");

sys.drawFunc(function () {
	if (!draw.isTexture(img)) {
		console.log("Loading Image 1");
		img = draw.openImage("image/testing.png");
	}
	
	draw.setColor("white");
	draw.print(10, 10, "Hello world this is a pretty long test to see what the hell is going on with font rendering");
});