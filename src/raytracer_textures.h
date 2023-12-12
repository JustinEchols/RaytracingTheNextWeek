#if !defined(RAYTRACER_TEXTURES_H)

#include <raytracer_image.h>

class texture
{
	public:
		virtual v3f value(f32 u, f32 v, const v3f &V) const = 0;
};

class texture_solid: public texture
{
	public:
		texture_solid(v3f Color);
		texture_solid(f32 R, f32 G, f32 B);
		v3f value(f32 u, f32 v, const v3f &V) const;

		v3f Value;
};

texture_solid::texture_solid(v3f Color)
{
	Value = Color;
}

texture_solid::texture_solid(f32 R, f32 G, f32 B)
{
	Value = v3f(R, G, B);
}

v3f
texture_solid::value(f32 u, f32 v, const v3f &V) const
{
	return(Value);
}

class texture_checkered: public texture
{
	public:
		texture_checkered(f32 scale, texture *TexEven, texture *TexOdd);
		texture_checkered(f32 scale, v3f ColorA, v3f ColorB);
		virtual v3f value(f32 u, f32 v, const v3f &V) const;

		f32 scale_inverse;
		texture *TextureEven;
		texture *TextureOdd;
};

texture_checkered::texture_checkered(f32 scale, texture *TexEven, texture *TexOdd)
{
	scale_inverse = 1.0f / scale;
	TextureEven = TexEven;
	TextureOdd = TexOdd;
}

texture_checkered::texture_checkered(f32 scale, v3f ColorA, v3f ColorB)
{
	scale_inverse = 1.0f / scale;
	TextureEven = new texture_solid(ColorA);
	TextureOdd = new texture_solid(ColorB);
}

v3f
texture_checkered::value(f32 u, f32 v, const v3f &V) const
{
	v3f Result;
	
	s32 x = (s32)(std::floor(scale_inverse * V.x()));
	s32 y = (s32)(std::floor(scale_inverse * V.y()));
	s32 z = (s32)(std::floor(scale_inverse * V.z()));

	b32 even = ((x + y + z) % 2 == 0);

	if(even)
	{
		Result = TextureEven->value(u, v, V);
	}
	else
	{
		Result = TextureOdd->value(u, v, V);
	}

	return(Result);
}

class texture_image: public texture
{
	public:
		texture_image(image *I) {Image = I;}
		virtual v3f value(f32 u, f32 v, const v3f &V) const;

		image *Image;
};

v3f
texture_image::value(f32 u, f32 v, const v3f &V) const
{
	v3f Result;

	if(Image->height <= 0)
	{
		Result = v3f(0.0f, 1.0f, 1.0f);
	}
	else
	{
		u = clamp(0.0f, u, 1.0f);
		v = 1.0f - clamp(0.0f, v, 1.0f);

		s32 x = (s32)(u * Image->width);
		s32 y = (s32)(v * Image->height);
		const unsigned char *pixel = Image->pixel(x, y);

		f32 scale = 1.0f / 255.0f;

		Result = scale * v3f(*pixel, *(pixel + 1), *(pixel + 2));

	}
	return(Result);
}


class texture_perlin: public texture
{
	public:
		texture_perlin() {}
		texture_perlin(f32 c) {scale = c;}
		virtual v3f value(f32 u, f32 v, const v3f &V) const;

		perlin Perlin;
		f32 scale;
};

v3f
texture_perlin::value(f32 u, f32 v, const v3f &V) const
{
	v3f Result;

	//f32 noise = 0.5f * (1.0f + Perlin.noise(scale * V));
	
	// NOTE(Justin): Trubulence only 
	//f32 noise = Perlin.turbulence(scale * V);

	// NOTE(Justin): Trubulence and phase shift
	v3f VScaled = scale * V;
	f32 noise = 0.5f * (1.0f + sin(VScaled.z() + 10.0f * Perlin.turbulence(VScaled)));

	Result = noise * v3f(1.0f, 1.0f, 1.0f);

	return(Result);
}

#define RAYTRACER_TEXTURES_H
#endif
