var buff = new draw.VertexBuffer2D("shaders/basic.json");
buff.load("models/torus.eglb")
buff.setDepthTest(true);
buff.setProjectionPerspective(true);
buff.setLookAtView(60.2, 60.2, 60.2, 0, 0, 0);
buff.model.scale(new Math.Vector(5, 5, 5));

var buff1 = buff.model.copy();
buff1.scale(new Math.Vector(5, 5, 5));
var buff2 = buff.model.copy();
buff2.scale(new Math.Vector(3, 3, 3)).rotate(Math.degToRad(90), new Math.Vector(1, 0, 0));
var buff3 = buff.model.copy();
buff3.scale(new Math.Vector(2, 2, 2));

event.draw = function ring3DTest_draw (e) {
	buff.setLookAtView(input.mouseX * 0.1, 60.2, input.mouseY * 0.1, 0, 0, 0);

	buff1.rotate(-sys.deltaTime, new Math.Vector(0, 1, 0));
	buff2.rotate(-sys.deltaTime * 1.5, new Math.Vector(0, 1, 0));
	buff3.rotate(-sys.deltaTime, new Math.Vector(0, 1, 0));
	buff.model = buff1;
	buff.draw();
	buff.model = buff2;
	buff.draw();
	buff.model = buff3;
	buff.draw();
};