#if !defined(RAYTRACER_RENDER_H)

#include <fstream>

class camera
{
	public:
		void init();

		ray ray_get(s32 x, s32 y) const;
		v3f ray_sample_square() const;
		v3f ray_color(object *Obj, const ray &Ray, u32 scatter_count) const;
		v3f defocus_disk_sample() const;
		void image_render(object *Obj, char *filename) const;

		f32 aspect_ratio = 1.0f;
		u32 image_width = 100;
		u32 sample_count = 10;
		u32 scatter_count = 10;

		f32 fov_vertical = 90.0f;
		v3f LookFrom = v3f(0.0f, 0.0f, -1.0f);
		v3f LookAt = v3f(0.0f, 0.0f, 0.0f);
		v3f Up = v3f(0.0f, 1.0f, 0.0f);

		f32 defocus_angle = 0.2f;
		f32 focus_dist = 10.0f;


		u32 image_height;

		f32 ViewPortHeight;
		f32 ViewPortWidth;

		v3f U;
		v3f V;
		v3f W;

		v3f ViewPortX;
		v3f ViewPortY;
		v3f dX;
		v3f dY;

		v3f CameraOrigin;
		v3f ViewPortUpperLeft;
		v3f Pixel00;

		f32 defocus_radius;
		v3f DiskX;
		v3f DiskY;

		v3f Background = v3f(0.7f, 0.8f, 1.0f);
};

void
camera::init()
{
	image_height = (u32)((f32)image_width / aspect_ratio);

	CameraOrigin = LookFrom;

	f32 theta = DEG_TO_RAD(fov_vertical);
	f32 h = focus_dist * tan(theta / 2.0f);

	ViewPortHeight = 2.0f * h;
	ViewPortWidth = ViewPortHeight * ((f32)image_width / (f32)image_height);

	W = unit_vector(LookFrom - LookAt);
	U = unit_vector(cross(Up, W));
	V = unit_vector(cross(W, U));

	ViewPortX = ViewPortWidth * U;
	ViewPortY = ViewPortHeight * (-V);

	dX = (1.0f / (f32)image_width) * ViewPortX;
	dY = (1.0f/ (f32)image_height) * ViewPortY;

	ViewPortUpperLeft = LookFrom - (focus_dist * W) - (0.5f * ViewPortX) - (0.5f * ViewPortY);
	Pixel00 = ViewPortUpperLeft + 0.5f * (dX + dY);

	defocus_radius = focus_dist * tan(DEG_TO_RAD(defocus_angle / 2.0f));

	DiskX = defocus_radius * U;
	DiskY = defocus_radius * V;
}

ray
camera::ray_get(s32 x, s32 y) const
{
	ray Result;

	v3f PixelXY = Pixel00;
	v3f X = (f32)x * dX;
	v3f Y = (f32)y * dY;
	PixelXY += X + Y;
	PixelXY += ray_sample_square();

	v3f RayOrigin = ((defocus_angle <= 0.0f) ? CameraOrigin : defocus_disk_sample());
	v3f RayDir = PixelXY - RayOrigin;
	f32 RayTime = random_unit();

	Result = ray(RayOrigin, RayDir, RayTime);

	return(Result);
}

inline v3f
camera::defocus_disk_sample() const
{
	v3f Result;

	v3f P = v3f_rand_in_unit_disk();

	Result = CameraOrigin + P[0] * DiskX + P[1] * DiskY;

	return(Result);
}

v3f
camera::ray_sample_square() const
{
	v3f Result;

	f32 randX = -0.5f + random_unit();
	f32 randY = -0.5f + random_unit();

	Result = (randX * dX) + (randY * dY);

	return(Result);
}


v3f
camera::ray_color(object *Obj, const ray &Ray, u32 scatter_count) const
{
	v3f Result = {};

	intersect_record IntersectRecord = {};
	if(Obj->ray_intersect(Ray, interval(0.001f, inf), IntersectRecord))
	{
		if(scatter_count > 0)
		{
			ray RayScattered;
			v3f Atten;
			v3f EmittedColor = IntersectRecord.Material->emitted(IntersectRecord.u, IntersectRecord.v, IntersectRecord.Pos);
			if(IntersectRecord.Material->ray_scatter(Ray, IntersectRecord, Atten, RayScattered))
			{
				Result = EmittedColor + v3f_hadamard(Atten, ray_color(Obj, RayScattered, scatter_count - 1));
			}
			else
			{
				Result = EmittedColor;
			}
		}
		else
		{
			Result = v3f(0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		Result = Background;
	}
	return(Result);
}

void
camera::image_render(object *Obj, char *filename) const
{
	// NOTE(Justin):
	// 1) Pixels are written out as rows.
	// 2) Pixels in a row are written from left to right.
	// 3) Rows are written from top to bottom.

	std::ofstream OutFileStream(filename);
	OutFileStream << "P3\n" << image_width << " " << image_height << "\n255\n";
	for(s32 y = 0; y < image_height; y++)
	{
		for(s32 x = 0; x < image_width; x++)
		{
			v3f Color = v3f(0.0f, 0.0f, 0.0f);
			for(u32 sample_index = 0; sample_index < sample_count; sample_index++)
			{
				ray Ray = ray_get(x, y);
				Color += ray_color(Obj, Ray, scatter_count);
			}
			Color = (1.0f / (f32)sample_count) * Color;
			Color = color_linear_to_gamma2(Color);

			u32 r = (u32)(256.0f * clamp(0.000f, Color[0], 0.999f));
			u32 g = (u32)(256.0f * clamp(0.000f, Color[1], 0.999f));
			u32 b = (u32)(256.0f * clamp(0.000f, Color[2], 0.999f));

			OutFileStream << r << " " << g << " " << b << "\n";
		}
	}
	OutFileStream.close();
}

#define RAYTRACER_RENDER_H
#endif
