#version 150
in vec3 vertex;
in vec4 color;
in vec2 texCoard;

uniform mat4 projection;

out vec4 postColor;
out vec2 postTexCoard;

void main() {
	postColor = color;
	postTexCoard = texCoard;
	gl_Position = projection * vec4(vertex, 1.0);
}