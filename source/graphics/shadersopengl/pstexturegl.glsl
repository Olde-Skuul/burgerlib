//
// Simple unlit texture shader OpenGL version
// Attach to a vertex array of positions
//

#ifdef GL_ES
precision highp float;
#endif

//
// Piped variables
//

PIPED vec2 vPipedTexcoord;		// Texture UV coordinates from the vertex

//
// Simple mapping
//

FRAGCOLOR_USED							// Outputs color
uniform sampler2D DiffuseTexture;		// Attached texture

void main(void) {
	// Simply get a texture pixel and draw it
	gl_FragColor = texture2D(DiffuseTexture,vPipedTexcoord);
}


