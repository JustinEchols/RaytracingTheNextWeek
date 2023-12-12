#if !defined(RAYTRACER_IMAGE_H)

#if defined(_MSC_VER)
	#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class image
{
	public:
		image() {contents = NULL;}
		b32 load(char *filename);
		s32 pixel_clamp(s32 x, s32 low, s32 high);
		const unsigned char *pixel(s32 x, s32 y) const;


		unsigned char *contents;
		s32 width;
		s32 height;
		s32 stride;
		const s32 bytes_per_pixel = 3;
};

b32
image::load(char *filename)
{
	b32 Result = false;
	s32 n = bytes_per_pixel;
	contents = stbi_load(filename, &width, &height, &n, bytes_per_pixel);
	stride = width * bytes_per_pixel;

	if(contents)
	{
		Result = true;
	}

	return(Result);
}

const unsigned char *
image::pixel(s32 x, s32 y) const
{
	const unsigned char *Result;

	if(contents == NULL)
	{
		static unsigned char magneta[] = {255, 0, 255};
		Result = magneta;
	}
	else
	{
		x = clamp(0, x, width - 1);
		y = clamp(0, y, height - 1);

		Result = contents + y * stride + x * bytes_per_pixel;
	}

	return(Result);
}

#if defined(_MSC_VER)
	#pragma warning (pop)
#endif

#define RAYTRACER_IMAGE_H
#endif
