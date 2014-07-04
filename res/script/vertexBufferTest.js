var buff = new draw.VertexBuffer2D("shaders/basic.json");

// add a triangle
buff.addVert(50, 50, new draw.Color("red"));
buff.addVert(250, 50, new draw.Color("green"));
buff.addVert(250, 250, new draw.Color("blue"));
buff.addVert(50, 50, new draw.Color("red"));
buff.addVert(50, 250, new draw.Color("green"));
buff.addVert(250, 250, new draw.Color("blue"));

event.draw = function vertexBufferTest_draw (e) {
	buff.draw();
};