varying vec4 postColor;
varying vec2 postTexCoard;
varying vec4 postVertex;

uniform sampler2D tex;

void main() {
	gl_FragColor = texture2D(tex, postTexCoard) * postColor;
}