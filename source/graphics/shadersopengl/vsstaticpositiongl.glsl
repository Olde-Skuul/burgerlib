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

PIPED vec2 vPipedTexcoord;			// Texture UV coordinates from the vertex

uniform mat4 ViewProjectionMatrix;	// 3D projection matrix
VERTEX_INPUT vec3 Position;		// Vertex from the array
VERTEX_INPUT vec2 Texcoord;		// Texture UV from the array

void main(void) {
	// Perform the standard 3D projection
	gl_Position = ViewProjectionMatrix * vec4(Position,1);
	// Pass the texture UV as is
	vPipedTexcoord = Texcoord;
}
