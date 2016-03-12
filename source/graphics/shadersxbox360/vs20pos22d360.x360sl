//
// Vertex shader for simple 2D graphics
// Attach to a vertex array set to 0.0 to 2.0f 
//

// X,Y, Width and Height from 0.0f to 1.0f
// to map to the screen size

float4 XYWidthHeight : register(c0);
static const float4 Adjustments = {2.0f,-2.0f,-1.0f,1.0f};

//
// Once the vertex shader is finished, it will 
// pass the vertices on to the pixel shader like this...
//

struct VS2PS {
	float4 gl_Position: POSITION0;		// Transformed vertex
};

//
// Simple Vertex Shader
//

VS2PS main(float2 inPosition : POSITION0) {
	VS2PS output;

	// Adjust the origin X/Y, and then add the scaled width and height
	output.gl_Position = float4(XYWidthHeight.xy*Adjustments.xy + Adjustments.zw + inPosition.xy*XYWidthHeight.zw,0,1.0f);
	return output;
}
