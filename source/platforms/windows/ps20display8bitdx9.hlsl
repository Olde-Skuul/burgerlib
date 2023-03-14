/***************************************

	Simple shader to take an 8 bit paletted image
	and blast it to the display screen using
	directx 9

	Copyright Rebecca Ann Heineman
	
***************************************/

// The image and palette will not change during the execution of the shader
// so they are declared as uniform

// Bitmap is the 2D 8 bit image, declared as a "D3DFMT_L8"
uniform sampler2D Bitmap;

// Palette256 is a 1D 256 entry color lookup table declared as "D3DFMT_X8R8G8B8"
uniform sampler1D Palette256;

//
// This function does a simple lookup of a texture pixel into the 1D array
//

float4 main(in float2 Coordinate:TEXCOORD0) : COLOR
{
	// Look up from Palette256 using just the "red" component as a lookup

	// Note: Note that the luminance (palette index) is adjusted with a
	// multiply-add operation. This is necessary, as palette index 255
	// is considered as white (maximum luminance), which becomes 1.0f
	// when represented as a float. Reading the palette texture at that
	// coordinate causes it to wrap around (as only the fractionary part
	// is used) and read the first palette entry instead.

	return tex1D(Palette256, tex2D(Bitmap,Coordinate).a); // * (255.0f/256.0f) + (0.5f/256.0f));
}