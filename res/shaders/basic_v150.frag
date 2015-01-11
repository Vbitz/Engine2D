in vec4 postColor;
in vec3 postTexCoard;
in vec4 postVertex;

out vec4 outColor;

uniform sampler2D tex1;

void main() {
	if (postTexCoard.z > 0.5 && postTexCoard.z < 1.5) { 
		outColor = texture(tex1, postTexCoard.xy) * postColor;
	} else if (postTexCoard.z > 1.5 && postTexCoard.z < 2.5) { 
		outColor = postColor;
	}
}