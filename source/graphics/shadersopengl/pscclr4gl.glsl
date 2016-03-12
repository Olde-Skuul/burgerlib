//
// Simple solid color shader OpenGL version
//

#ifdef GL_ES
precision highp float;
#endif

//
// Simple mapping
//

FRAGCOLOR_USED							// Outputs color
uniform vec4 Color;						// Color multiplier

void main(void) {
	// Simply get a color pixel and draw it
	gl_FragColor = Color;
}
