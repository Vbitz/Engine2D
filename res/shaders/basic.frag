#version 150
in vec4 postColor;
in vec2 postTexCoard;
in vec4 postVertex;

out vec4 outColor;

void main() {
	outColor = postColor + sin(postVertex / 2);
}