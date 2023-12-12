#if !defined(RAYTRACER_MATERIALS_H)

#include <raytracer_ray.h>
#include <raytracer_textures.h>

class material
{
	public:
		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const = 0;
		virtual v3f emitted(f32 u, f32 v, const v3f &V) const {return(v3f(0.0f, 0.0f, 0.0f));}
};

class lambertian: public material
{
	public:
		lambertian(texture *Tex) {Albedo = Tex;}

		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const;

		texture *Albedo;
};

b32
lambertian::ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const
{
	v3f Dir = IntersectRecord.Normal + v3f_rand_unit_vector();
	if(v3f_near_zero(Dir))
	{
		Dir = IntersectRecord.Normal;
	}

	RayScattered = ray(IntersectRecord.Pos, Dir, RayIn.time);
	Atten = Albedo->value(IntersectRecord.u, IntersectRecord.v, IntersectRecord.Pos);

	return(true);
}

class metal: public material
{
	public:
		metal(const v3f &A, f32 f);

		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const;

		v3f Albedo;
		f32 fuzz;
};

metal::metal(const v3f &A, f32 f)
{
	Albedo = A;
	fuzz = ((f < 1.0f) ? f : 1.0f);
}

b32
metal::ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const
{
	v3f R = reflect(RayIn.Direction, IntersectRecord.Normal);
	R += fuzz * v3f_rand_unit_vector();
	RayScattered = ray(IntersectRecord.Pos, R, RayIn.time);
	Atten = Albedo;

	return(dot(RayScattered.Direction, IntersectRecord.Normal) > 0.0f);
	//return(true);
}

class dielectric: public material
{
	public:
		dielectric(f32 refract_index);

		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const;

		f32 refraction_index;
};

dielectric::dielectric(f32 refract_index)
{
	refraction_index = refract_index;
}

b32
dielectric::ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const
{
	Atten = v3f(1.0f, 1.0f, 1.0f);

	f32 ratio = (IntersectRecord.front_face ? (1.0f / refraction_index) : refraction_index);

	f32 cos_theta = MIN(dot(-RayIn.Direction, IntersectRecord.Normal), 1.0f);
	f32 sin_theta = sqrt(1.0f - SQUARE(cos_theta));

	b32 cannot_refract = ((sin_theta * ratio) > 1.0f);

	v3f Dir;
	if((cannot_refract) || (reflectance(cos_theta, ratio) > random_unit()))
	{
		Dir = reflect(RayIn.Direction, IntersectRecord.Normal);
	}
	else
	{
		Dir = refract(RayIn.Direction, IntersectRecord.Normal, ratio);
	}

	RayScattered = ray(IntersectRecord.Pos, Dir, RayIn.time);

	return(true);


}

class diffuse_light: public material
{
	public:
		diffuse_light(texture *Tex) {Emit = Tex;}
		diffuse_light(v3f Color) {Emit = new texture_solid(Color);}

		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const {return(false);}
		v3f virtual emitted(f32 u, f32 v, const v3f &V) const {return(Emit->value(u, v, V));};

		texture *Emit;
};

#define RAYTRACER_MATERIALS_H
#endif
