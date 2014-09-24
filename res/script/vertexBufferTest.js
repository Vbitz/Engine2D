fs.configDir("vbTest");

var buff = new draw.VertexBuffer2D("shaders/basic.json");
buff.setDepthTest(true);
buff.setProjectionPerspective(true);
buff.setLookAtView(10.2, 10.2, 10.2, 0, 0, 0);

// add a triangle
buff.addVert(-1, -1, new draw.Color("purple"));
buff.addVert(1, -1, new draw.Color("green"));
buff.addVert(1, 1, new draw.Color("blue"));
buff.addVert(-1, -1, new draw.Color("purple"));
buff.addVert(-1, 1, new draw.Color("green"));
buff.addVert(1, 1, new draw.Color("blue"));

event.draw = function vertexBufferTest_draw (e) {
	buff.model.rotate(sys.deltaTime, new Math.Vector(0, 1, 0));
	buff.draw();
};

event.key_S = function vertexBufferTest_save (e) {
	buff.save("testing.eglb");
	console.log("Saved");
};

event.key_L = function vertexBufferTest_load (e) {
	buff.load("testing.eglb");
	console.log("Loaded");
	buff.model.reset();
	buff.model.rotate(Math.degToRad(90), new Math.Vector(1, 0, 0));
	buff.computeNormals();
};