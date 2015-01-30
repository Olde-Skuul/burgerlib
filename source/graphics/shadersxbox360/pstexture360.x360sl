//
// Simple unlit texture shader DirectX9 version
// Attach to a vertex array of positions
//

sampler DiffuseTexture : register(s0);	// Attached texture

float4 main(float2 vPipedTexcoord : TEXCOORD0) : COLOR0 {
	// Simply get a texture pixel and draw it
	return tex2D(DiffuseTexture,vPipedTexcoord);
}
