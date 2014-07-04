fs.configDir("vbTest");

var buff = new draw.VertexBuffer2D("shaders/basic.json");

var saved = false;

// add a triangle
buff.addVert(25, 50, new draw.Color("purple"));
buff.addVert(250, 50, new draw.Color("green"));
buff.addVert(250, 250, new draw.Color("blue"));
buff.addVert(25, 50, new draw.Color("purple"));
buff.addVert(50, 250, new draw.Color("green"));
buff.addVert(250, 250, new draw.Color("blue"));

event.draw = function vertexBufferTest_draw (e) {
	buff.draw();
};

event.key_S = function vertexBufferTest_save (e) {
	buff.save("testing.eglb");
	console.log("Saved");
};

event.key_L = function vertexBufferTest_load (e) {
	buff.load("testing.eglb");
	console.log("Loaded");
};