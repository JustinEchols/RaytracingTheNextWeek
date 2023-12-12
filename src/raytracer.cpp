#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u32 b32;

typedef float f32;
typedef double f64;

#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include <raytracer_hitable.h>
#include <raytracer_hitable_list.h>
#include <raytracer_geometry.h>
#include <raytracer_materials.h>
#include <raytracer_particles.h>
#include <raytracer_bvh.h>
#include <raytracer_textures.h>
#include <raytracer_image.h>
#include <raytracer_renderer.h>

object_list
world_final_2()
{
	object_list Result;

	object_list Boxes1;
	material *GroundMaterial = new lambertian(new texture_solid(v3f(0.48f, 0.83f, 0.53f)));

	u32 boxes_per_side = 20;
	for(u32 i = 0; i < boxes_per_side; i++)
	{
		for (u32 j = 0; j < boxes_per_side; j++)
		{
			f32 w = 100.0f;
			f32 x0 = -1000.0f + (f32)i * w;
			f32 z0 = -1000.0f + (f32)j * w;
			f32 y0 = 0.0f;
			f32 x1 = x0 + w;
			f32 y1 = random_interval(1.0f, 101.0f);
			f32 z1 = z0 + w;

			object *Box = new box(v3f(x0, y0, z0), v3f(x1, y1, z1), GroundMaterial);
			Boxes1.add(Box);
		}
	}


	Result.add_list(Boxes1);

	material *Light = new diffuse_light(v3f(7.0f, 7.0f, 7.0f));
	Result.add(new quad(v3f(123.0f, 554.0f, 147.0f), v3f(300.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, 265.0f), Light));

	v3f Center1 = v3f(400.0f, 400.0f, 200.0f);
	v3f Center2 = Center1 + v3f(30.0f, 0.0f, 0.0f);

	material *SphereMaterial = new lambertian(new texture_solid(v3f(0.7f, 0.3f, 0.1f)));
	Result.add(new sphere(Center1, Center2, 50.0f, SphereMaterial));

    Result.add(new sphere(v3f(260.0f, 150.0f, 45.0f), 50.0f, new dielectric(1.5)));
    Result.add(new sphere(v3f(0.0f, 150.0f, 145.0f), 50.0f, new metal(v3f(0.8f, 0.8f, 0.9f), 1.0f)));

    object *Boundary = new sphere(v3f(360.0f, 150.0f, 145.0f), 70.0f, new dielectric(1.5));

    Result.add(Boundary);
    Result.add(new medium_constant(Boundary, 0.2f, v3f(0.2f, 0.4f, 0.9f)));

    Boundary = new sphere(v3f(0.0f, 0.0f,0.0f), 5000.0f, new dielectric(1.5));
    Result.add(new medium_constant(Boundary, 0.0001f, v3f(1.0f, 1.0f,1.0f)));

	image *Image = new image;
	Image->load("..\\data\\earthmap.jpg");

	texture *EarthTexture = new texture_image(Image);
	material *EarthSurface = new lambertian(EarthTexture);

    Result.add(new sphere(v3f(400.0f, 200.0f, 400.0f), 100.0f, EarthSurface));

	texture *PerlinTexture = new texture_perlin(4.0f);
	material *Perlin = new lambertian(PerlinTexture);

    Result.add(new sphere(v3f(220.0f, 280.0f, 300.0f), 80.0f, Perlin));

    object_list Boxes2;
	material *White = new lambertian(new texture_solid(v3f(0.73f, 0.73f, 0.73f)));


    u32 ns = 1000;
    for (u32 j = 0; j < ns; j++)
	{
        Boxes2.add(new sphere(v3f_rand_interval(0.0f, 165.0f), 10.0f, White));
    }


	object *SphereBox = Boxes2.Objects[0];
    Result.add(new translate(new rotate_y(SphereBox, 15.0f), v3f(-100.0f, 270.0f, 395.0f)));

	return(Result);
}

object_list
world_cornell_smoke()
{
	object_list Result;

	material *Red   = new lambertian(new texture_solid(v3f(0.65f, 0.05f, 0.05f)));
	material *White = new lambertian(new texture_solid(v3f(0.73f, 0.73f, 0.73f)));
	material *Green = new lambertian(new texture_solid(v3f(0.12f, 0.45f, 0.15f)));
	material *Light = new diffuse_light(v3f(15.0f, 15.0f, 15.0f));

	Result.add(new quad(v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), Green));
	Result.add(new quad(v3f(0.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), Red));
	Result.add(new quad(v3f(343.0f, 554.0f, 332.0f), v3f(-130.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, -105.0f), Light));
	Result.add(new quad(v3f(0.0f, 0.0f, 0.0f), v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), White));
	Result.add(new quad(v3f(555.0f, 555.0f, 555.0f), v3f(-555.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f,-555.0f), White));
	Result.add(new quad(v3f(0.0f, 0.0f, 555.0f), v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), White));

	v3f BoxP0 = v3f(0.0f, 0.0f, 0.0f);
	v3f BoxP1 = v3f(165.0f, 330.0f, 165.0f);
	v3f Delta1 = v3f(265.0f, 0.0f, 295.0f);

	v3f BoxP3 = v3f(165.0f, 165.0f, 165.0f);
	v3f Delta2 = v3f(130.0f, 0.0f, 65.0f);

	object *Box1 = new box(BoxP0, BoxP1, White);
	Box1 = new rotate_y(Box1, 15.0f);
	Box1 = new translate(Box1, Delta1);

	object *Box2 = new box(BoxP0, BoxP3, White);
	Box2 = new rotate_y(Box2, -18.0f);
	Box2 = new translate(Box2, Delta2);
	
	object *SmokeyBox1 = new medium_constant(Box1, 0.01f, v3f(0.0f, 0.0f, 0.0f));
	object *SmokeyBox2 = new medium_constant(Box2, 0.01f, v3f(1.0f, 1.0f, 1.0f));

	Result.add(SmokeyBox1);
	Result.add(SmokeyBox2);

	return(Result);
}

object_list
world_cornell_box()
{
	object_list Result;
	material *Red   = new lambertian(new texture_solid(v3f(0.65f, 0.05f, 0.05f)));
	material *White = new lambertian(new texture_solid(v3f(0.73f, 0.73f, 0.73f)));
	material *Green = new lambertian(new texture_solid(v3f(0.12f, 0.45f, 0.15f)));
	material *Light = new diffuse_light(v3f(15.0f, 15.0f, 15.0f));

	Result.add(new quad(v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), Green));
	Result.add(new quad(v3f(0.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), Red));
	Result.add(new quad(v3f(343.0f, 554.0f, 332.0f), v3f(-130.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, -105.0f), Light));
	Result.add(new quad(v3f(0.0f, 0.0f, 0.0f), v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, 555.0f), White));
	Result.add(new quad(v3f(555.0f, 555.0f, 555.0f), v3f(-555.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f,-555.0f), White));
	Result.add(new quad(v3f(0.0f, 0.0f, 555.0f), v3f(555.0f, 0.0f, 0.0f), v3f(0.0f, 555.0f, 0.0f), White));

	v3f BoxP0 = v3f(0.0f, 0.0f, 0.0f);
	v3f BoxP1 = v3f(165.0f, 330.0f, 165.0f);
	v3f Delta1 = v3f(265.0f, 0.0f, 295.0f);

	v3f BoxP3 = v3f(165.0f, 165.0f, 165.0f);
	v3f Delta2 = v3f(130.0f, 0.0f, 65.0f);

	object *Box1 = new box(BoxP0, BoxP1, White);
	Box1 = new rotate_y(Box1, 15.0f);
	Box1 = new translate(Box1, Delta1);

	object *Box2 = new box(BoxP0, BoxP3, White);
	Box2 = new rotate_y(Box2, -18.0f);
	Box2 = new translate(Box2, Delta2);
	
	Result.add(Box1);
	Result.add(Box2);

	return(Result);
}
object_list
world_simple_light()
{
	object_list Result;

	texture *PerlinTexture = new texture_perlin(4.0f);
	material *Perlin = new lambertian(PerlinTexture);

	Result.add(new sphere(v3f(0.0f, -1000.0f, 0.0f), 1000.0f, Perlin));
	Result.add(new sphere(v3f(0.0f, 2.0f, 0.0f), 2.0f, Perlin));

	material *DiffuseLight = new diffuse_light(v3f(4.0f, 4.0f, 4.0f));
	Result.add(new sphere(v3f(0.0f, 7.0f, 0.0f), 2.0f, DiffuseLight));
	Result.add(new quad(v3f(3.0f, 1.0f, -2.0f), v3f(2.0f, 0.0f, 0.0f), v3f(0.0f, 2.0f, 0.0f), DiffuseLight));

	return(Result);
}

#if 1
object_list
world_quads()
{
	object_list Result;

	material *Red = new lambertian(new texture_solid(v3f(1.0f, 0.2f, 0.2f)));
	material *Green = new lambertian(new texture_solid(v3f(0.2f, 1.0f, 0.2f)));
	material *Blue = new lambertian (new texture_solid(v3f(0.2f, 0.2f, 1.0f)));
	material *Orange = new lambertian(new texture_solid(v3f(1.0f, 0.5, 0.0f)));
	material *Teal = new lambertian(new texture_solid(v3f(0.2f, 0.8, 0.8f)));

	Result.add(new quad(v3f(-3.0f, -2.0f, 5.0f), v3f(0.0, 0.0f, -4.0f), v3f(0.0f, 4.0f, 0.0f), Red));
	Result.add(new quad(v3f(-2.0f, -2.0f, 0.0f), v3f(4.0f, 0.0f, 0.0f), v3f(0.0f, 4.0f, 0.0f), Green));
	Result.add(new quad(v3f(3.0f, -2.0f, 1.0f),  v3f(0.0f, 0.0f, 4.0f), v3f(0.0f, 4.0f, 0.0f), Blue));
	Result.add(new quad(v3f(-2.0f, 3.0f, 1.0f),  v3f(4.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, 4.0f), Orange));
	Result.add(new quad(v3f(-2.0f, -3.0f, 5.0f), v3f(4.0f, 0.0f, 0.0f), v3f(0.0f, 0.0f, -4.0f), Teal));

#if 0
	object *Root = new bvh_node(Result);
	Result.add(Root);
#else
#endif

	return(Result);
}


object_list
world_perlin_spheres()
{
	object_list Result;

	texture *PerlinTexture = new texture_perlin(4.0f);
	material *Perlin = new lambertian(PerlinTexture);

	Result.add(new sphere(v3f(0.0f, -1000.0f, 0.0f), 1000.0f, Perlin));
	Result.add(new sphere(v3f(0.0f, 2.0f, 0.0f), 2.0f, Perlin));

#if 0
	object *Root = new bvh_node(Result);
	Result.add(Root);
#else
#endif

	return(Result);
}
#endif

object_list
world_earth()
{
	object_list Result;

	image *Image = new image;
	Image->load("..\\data\\earthmap.jpg");

	texture *EarthTexture = new texture_image(Image);
	material *EarthSurface = new lambertian(EarthTexture);

	object_list ObjList;
	ObjList.add(new sphere(v3f(0.0f, 0.0f, 0.0f), 2.0f, EarthSurface));

#if 0
	object *Root = new bvh_node(ObjList.Objects, 0, ObjList.Objects.size());
	Result.add(Root);
#else
	Result = ObjList;
#endif

	return(Result);
}


object_list
world_checkered_spheres()
{
	object_list Result;

	texture *TextureCheckered = new texture_checkered(0.8f, v3f(0.2f, 0.3f, 0.1f), v3f(0.9f, 0.9f, 0.9f));
	material *SphereMaterial = new lambertian(TextureCheckered);

	Result.add(new sphere(v3f(0.0f, -10.0f, 0.0f), 10.0f, SphereMaterial));
	Result.add(new sphere(v3f(0.0f, 10.0f, 0.0f), 10.0f, SphereMaterial));

#if 0
	object *Root = new bvh_node(Result);
	Result.add(Root);
#else
#endif

	return(Result);
}

object_list
world_checkered_ground()
{
	object_list Result;

	texture_checkered *TextureCheckered = new texture_checkered(0.32f, v3f(0.2f, 0.3f, 0.1f), v3f(0.9f, 0.9f, 0.9f));
	material *MaterialGround = new lambertian(TextureCheckered);
	Result.add(new sphere(v3f(0.0f, -1000.0f, 0.0f), 1000.0f, MaterialGround));

	for(s32 x = -11; x < 11; x++)
	{
		for(s32 y = -11; y < 11; y++)
		{
			f32 choose_mat = random_unit();
			v3f Center = v3f(x + 0.9f * random_unit(), 0.2f, y + 0.9f * random_unit());

			if((Center - v3f(4.0f, 0.2f, 0.0f)).length() > 0.9f)
			{
				material *SphereMaterial;
				if(choose_mat < 0.8f)
				{
					texture *Albedo = new texture_solid(v3f_hadamard(v3f_rand(), v3f_rand()));
					SphereMaterial = new lambertian(Albedo);
					v3f C1 = Center + v3f(0.0f, random_interval(0.0f, 0.5f), 0.0f);
					Result.add(new sphere(Center, C1, 0.2f, SphereMaterial));
				}
				else if(choose_mat < 0.95f)
				{
					v3f Albedo = v3f(random_interval(0.5f, 1.0f), random_interval(0.5f, 1.0f), random_interval(0.5f, 1.0f));
					f32 fuzzy = random_interval(0.0f, 0.5f);
					SphereMaterial = new metal(Albedo, fuzzy);
					Result.add(new sphere(Center, 0.2f, SphereMaterial));
				}
				else
				{
					SphereMaterial = new dielectric(1.5);
					Result.add(new sphere(Center, 0.2f, SphereMaterial));
				}
			}
		}
	}

	material *Material1 = new dielectric(1.5f);
	material *Material2 = new lambertian(new texture_solid(v3f(0.4f, 0.2f, 0.1f)));
	material *Material3 = new metal(v3f(0.7f, 0.6f, 0.5f), 0.0f);

	Result.add(new sphere(v3f(0.0f, 1.0f, 0.0f), 1.0f, Material1));
	Result.add(new sphere(v3f(-4.0f, 1.0f, 0.0f), 1.0f, Material2));
	Result.add(new sphere(v3f(4.0f, 1.0f, 0.0f), 1.0f, Material3));

#if 0
	object *Root = new bvh_node(Result);
	Result.add(Root);
#else
#endif

	return(Result);
}

object_list
world_defocus_blur()
{
	object_list Result;

	texture *GroundTexture = new texture_solid(v3f(0.8f, 0.8f, 0.0f));
	texture *SphereTexture = new texture_solid(v3f(0.1f, 0.2f, 0.5f));

	material *MaterialGround = new lambertian(GroundTexture);
	material *MaterialCenter = new lambertian(SphereTexture);
	material *MaterialLeft = new dielectric(1.5f);
	material *MaterialRight = new metal(v3f(0.8f, 0.6f, 0.2f), 0.0f);

	Result.add(new sphere(v3f(0.0f, -100.5f, -1.0f), 100.0f, MaterialGround));
	Result.add(new sphere(v3f(0.0f, 0.0f, -1.0f), 0.5f, MaterialCenter));
	Result.add(new sphere(v3f(-1.0f, 0.0f, -1.0f), 0.5f, MaterialLeft));
	Result.add(new sphere(v3f(-1.0f, 0.0f, -1.0f), -0.4f, MaterialLeft));
	Result.add(new sphere(v3f(1.0f, 0.0f, -1.0f), 0.5f, MaterialRight));

#if 0
	object *Root = new bvh_node(Result);
	Result.add(Root);
#else
#endif

	return(Result);
}

object_list
world_depth_of_field()
{
	object_list Result;

	return(Result);
}

object_list
world_two_spheres_diffuse()
{
	object_list Result;

	texture *TextureDiffuse = new texture_solid(v3f(0.5f, 0.5f, 0.5f));
	material *SphereMaterial = new lambertian(TextureDiffuse);

	object_list ObjList;

	ObjList.add(new sphere(v3f(0.0f, 0.0f, -1.0f), 0.5f, SphereMaterial));
	ObjList.add(new sphere(v3f(0.0f, -100.5f, -1.0f), 100.0f, SphereMaterial));

#if 0
	object *Root = new bvh_node(ObjList.Objects, 0, ObjList.Objects.size());
	Result.add(Root);
#else
	Result = ObjList;
#endif

	return(Result);

}

s32
str_to_int(char *s)
{
	s32 Result = 0;
	for(char *c = s; *c != '\0'; c++)
	{
		Result = Result * 10 + *c - '0';
	}
	return(Result);
}

int main(int argc, char **argv)
{
	char *filename = "test.ppm";

	char *number = *(argv + argc - 1);
	s32 scene = str_to_int(number);

	object_list World;
	camera Camera;
	switch(scene)
	{
		case 1:
		{
			World = world_two_spheres_diffuse();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 90.0f;

			Camera.LookFrom = v3f(0.0f, 0.0f, 1.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;
			Camera.focus_dist = 1.0f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.init();
		} break;
		case 2:
		{
			World = world_defocus_blur();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(-2.0f, 2.0f, 1.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, -1.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 10.0f;
			Camera.focus_dist = 3.4f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.init();
		} break;
		case 3:
		{
			World = world_checkered_ground();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(13.0f, 2.0f, 3.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.02f;
			Camera.focus_dist = 10.0f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.init();


		} break;
		case 4:
		{
			World = world_earth();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(0.0f, 0.0f, 12.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.00f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.7f, 0.8f, 1.0f);

			Camera.init();
		} break;
		case 5:
		{
			World = world_checkered_spheres();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(13.0f, 2.0f, 3.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.7f, 0.8f, 1.0f);
			Camera.init();

		} break;
		case 6:
		{
			World = world_perlin_spheres();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0 / 9.0;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(13.0f, 2.0f, 3.0f);
			Camera.LookAt = v3f(0.0f, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.02f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.7f, 0.8f, 1.0f);
			Camera.init();

		} break;

		case 7:
		{
			World = world_quads();

			Camera.image_width = 400;
			Camera.aspect_ratio = 1.0f;

			Camera.fov_vertical = 80.0f;

			Camera.LookFrom = v3f(0.0f, 0.0f, 9.0f);
			Camera.LookAt = v3f(0.0, 0.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.7f, 0.8f, 1.0f);
			Camera.init();
		} break;
		case 8:
		{
			World = world_simple_light();

			Camera.image_width = 400;
			Camera.aspect_ratio = 16.0f / 9.0f;

			Camera.fov_vertical = 20.0f;

			Camera.LookFrom = v3f(26.0f, 3.0f, 6.0f);
			Camera.LookAt = v3f(0.0, 2.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.sample_count = 100;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.0f, 0.0f, 0.0f);

			Camera.init();
		} break;
		case 9:
		{
			World = world_cornell_box();

			Camera.image_width = 600;
			Camera.aspect_ratio = 16.0f / 9.0f;

			Camera.fov_vertical = 40.0f;

			Camera.LookFrom = v3f(278.0f, 278.0f, -800.0f);
			Camera.LookAt = v3f(278.0f, 278.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.sample_count = 200;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.0f, 0.0f, 0.0f);

			Camera.init();
		} break;
		case 10:
		{
			World = world_cornell_smoke();

			Camera.image_width = 600;
			Camera.aspect_ratio = 16.0f / 9.0f;

			Camera.fov_vertical = 40.0f;

			Camera.LookFrom = v3f(278.0f, 278.0f, -800.0f);
			Camera.LookAt = v3f(278.0f, 278.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.sample_count = 200;
			Camera.scatter_count = 50;

			Camera.Background = v3f(0.0f, 0.0f, 0.0f);

			Camera.init();
		} break;
		case 11:
		{
			World = world_final_2();

			Camera.aspect_ratio = 1.0;
			Camera.image_width = 800;
			Camera.sample_count = 2;
			Camera.scatter_count = 40;
			Camera.Background = v3f(0.0f, 0.0f, 0.0f);

			Camera.fov_vertical = 40;
			Camera.LookFrom = v3f(478.0f, 278.0f, -600.0f);
			Camera.LookAt = v3f(278.0f, 278.0f, 0.0f);
			Camera.Up = v3f(0.0f, 1.0f, 0.0f);

			Camera.defocus_angle = 0.0f;

			Camera.init();
		}
		default:
		{
		} break;
	}

	Camera.image_render(&World, filename);

	return 0;
}

