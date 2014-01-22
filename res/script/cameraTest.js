var i = 0;

sys.clearEvent("cameraTest.draw");
sys.on("draw", "cameraTest.draw", function (e) {
	draw.cameraReset();
	draw.cameraRotate(i++);
	draw.setColor("blue");
	draw.rect(0, 0, 100, 100);
});