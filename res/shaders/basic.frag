in vec4 postColor;
in vec2 postTexCoard;
in vec4 postVertex;

out vec4 outColor;

uniform sampler2D tex;

void main() {
	outColor = texture(tex, postTexCoard) * postColor;
}