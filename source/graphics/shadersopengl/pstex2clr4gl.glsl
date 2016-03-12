//
// Simple color adjusted texture shader OpenGL version
// Input Texture UV (2)
// Input Color multiplier (4)
//

#ifdef GL_ES
precision highp float;
#endif

//
// Piped variables
//

PIPED vec2 vPipedTexcoord;		// Texture UV coordinates from the vertex
PIPED vec4 vPipedColor;			// Color to multiply to the pixel

//
// Simple mapping
//

FRAGCOLOR_USED							// Outputs color
uniform sampler2D DiffuseTexture;		// Attached texture

void main(void) {
	// Simply get a texture pixel and draw it
	gl_FragColor = texture2D(DiffuseTexture,vPipedTexcoord)*vPipedColor;
}
