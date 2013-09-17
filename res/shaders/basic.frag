#version 150
in vec3 postColor;
out vec4 outColor;

void main() {
	outColor = vec4(postColor, 1.0);
}