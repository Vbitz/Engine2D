#version 150
in vec4 postColor;
in vec2 postTexCoard;

out vec4 outColor;

void main() {
	outColor = postColor;
}