attribute vec3 vertex;
attribute vec4 color;
attribute vec3 texCoard;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec4 postColor;
varying vec3 postTexCoard;
varying vec4 postVertex;

void main() {
	postColor = color;
	postTexCoard = texCoard;
	postVertex = (projection * view * model) * vec4(vertex, 1.0);
	gl_Position = postVertex;
}