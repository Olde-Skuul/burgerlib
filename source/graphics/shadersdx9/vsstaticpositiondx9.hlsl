//
// Simple unlit texture shader DirectX9 version
// Attach to a vertex array of positions
//

float4x4 ViewProjectionMatrix : register(c0);	// This matrix will be loaded by the application

//
// Once the vertex shader is finished, it will 
// pass the vertices on to the pixel shader like this...
//

struct VS2PS {
	float4 gl_Position: POSITION0;		// Transformed vertex
	float2 vPipedTexcoord: TEXCOORD0;	// Texture UV coordinates from the vertex
};

//
// Simple Vertex Shader
//

VS2PS main(float3 Position : POSITION0,float2 inTexcoord : TEXCOORD0) {
	VS2PS output;
	// Perform the standard 3D projection
	output.gl_Position = mul(float4(Position,1.0f),ViewProjectionMatrix);
	// Pass the texture UV as is
	output.vPipedTexcoord = inTexcoord;
	return output;
}
