/***************************************

	Simple shader to take an 8 bit paletted image
	and blast it to the display screen using
	directx 9 for the xbox 360

	Copyright Rebecca Ann Heineman

***************************************/

//
// Vertex structure passed into this shader
//
struct VSIN_SIMPLE
{
	float2 Position:POSITION;
	float2 UV:TEXCOORD0;
};

//
// Internal format for the Pixel Shader
//

struct VSOUT_SIMPLE
{
	float4 Position:POSITION;
	float2 TexCoord0:TEXCOORD0;
};

//
// Simple 2D textured vertex shader
//

VSOUT_SIMPLE Graphics8BitVS(VSIN_SIMPLE In)
{	
	VSOUT_SIMPLE  Output;
    // Transform the vertex
	Output.Position.x = In.Position.x;
	Output.Position.y = In.Position.y;
	Output.Position.z = 0.0f;
	Output.Position.w = 1.0f;
	Output.TexCoord0 = In.UV;
	return Output;
}

// The image and palette will not change during the execution of the shader
// so they are declared as uniform

// Bitmap is the 2D 8 bit image, declared as a "D3DFMT_LIN_L8"
uniform extern sampler2D Bitmap : register( s0 );

// Palette256 is a 1D 256 entry color lookup table declared as "D3DFMT_X8R8G8B8"
uniform extern sampler1D Palette256  : register( s1 );

//
// This function does a simple lookup of a texture pixel into the 1D array
//

float4 Graphics8BitPS(float2 TexCoord0:TEXCOORD0) : COLOR
{
	// Look up from Palette256 using just the "red" component as a lookup

	// Note: Note that the luminance (palette index) is adjusted with a
	// multiply-add operation. This is necessary, as palette index 255
	// is considered as white (maximum luminance), which becomes 1.0f
	// when represented as a float. Reading the palette texture at that
	// coordinate causes it to wrap around (as only the fractionary part
	// is used) and read the first palette entry instead.

	return tex1D(Palette256, tex2D(Bitmap,TexCoord0).r); // * (255.0f/256.0f) + (0.5f/256.0f));
}