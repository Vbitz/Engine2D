varying vec4 postColor;
varying vec3 postTexCoard;
varying vec4 postVertex;

uniform sampler2D tex1;

void main() {
	if (postTexCoard.z > 0.5 && postTexCoard.z < 1.5) { 
		gl_FragColor = texture2D(tex1, postTexCoard.xy) * postColor;
	} else if (postTexCoard.z > 1.5 && postTexCoard.z < 2.5) { 
		gl_FragColor = postColor;
	}
}