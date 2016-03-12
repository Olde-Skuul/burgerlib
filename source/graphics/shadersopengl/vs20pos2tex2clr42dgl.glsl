//
// Vertex shader for simple 2D graphics
// Attach to a vertex array set to 0.0 to 2.0f
//

#ifdef GL_ES
precision highp float;
#endif

//
// Piped variables
//

PIPED vec2 vPipedTexcoord;		// Texture UV coordinates from the vertex
PIPED vec4 vPipedColor;			// Color to multiply to the pixel

// X,Y, Width and Height from 0.0f to 1.0f
// to map to the screen size

uniform vec4 XYWidthHeight;
const vec4 Adjustments = vec4(2.0,-2.0,-1.0,1.0);

VERTEX_INPUT vec2 Position;			// Vertex from the array
VERTEX_INPUT vec2 Texcoord;			// Texture UV from the array
VERTEX_INPUT vec4 Color;			// Color from the array

void main(void) {
	// Perform the standard 3D projection
	gl_Position = vec4(XYWidthHeight.xy*Adjustments.xy + Adjustments.zw + Position*XYWidthHeight.zw,0,1.0);
	// Pass the texture UV as is
	vPipedTexcoord = Texcoord;
	// Pass the color as is
	vPipedColor = Color;
}
