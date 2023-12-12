#if !defined(RAYTRACER_PARTICLES_H)

#include <raytracer_hitable.h>
#include <raytracer_materials.h>
#include <raytracer_textures.h>

class isotropic: public material
{
	public:
		isotropic(v3f C);
		isotropic(texture *T);
		virtual b32 ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const;

		texture *Albedo;

};

isotropic::isotropic(v3f C)
{
	Albedo = new texture_solid(C);
}

isotropic::isotropic(texture *T)
{
	Albedo = T;
}

b32
isotropic::ray_scatter(const ray &RayIn, const intersect_record &IntersectRecord, v3f &Atten, ray &RayScattered) const
{
	b32 Result = true;

	RayScattered = ray(IntersectRecord.Pos, v3f_rand_unit_vector(), RayIn.time);
	Atten = Albedo->value(IntersectRecord.u, IntersectRecord.v, IntersectRecord.Pos);

	return(Result);
}

class medium_constant: public object
{
	public:
		medium_constant(object *O, f32 d, texture *Tex);
		medium_constant(object *O, f32 d, v3f Color);
		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;
		virtual aabb bounding_box() const;


		object *Boundary;
		f32 neg_inv_density;
		material *PhaseFunction;
};

medium_constant::medium_constant(object *O, f32 d, texture *Tex)
{
	Boundary = O;
	neg_inv_density = -1.0f / d;

	PhaseFunction = new isotropic(Tex);
}

medium_constant::medium_constant(object *O, f32 d, v3f Color)
{
	Boundary = O;
	neg_inv_density = -1.0f / d;

	PhaseFunction = new isotropic(Color);
}

b32
medium_constant::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;
	
	intersect_record Rec1, Rec2;

	if(Boundary->ray_intersect(Ray, interval(-inf, inf), Rec1))
	{
		if(Boundary->ray_intersect(Ray, interval(Rec1.t + 0.0001f, inf), Rec2))
		{
			if(Rec1.t < RayInterval.min)
			{
				Rec1.t = RayInterval.min;
			}
			if(Rec2.t > RayInterval.max)
			{
				Rec2.t = RayInterval.max;
			}
		
			if(Rec1.t < Rec2.t)
			{
				if(Rec1.t < 0.0f)
				{
					Rec1.t = 0.0f;
					
				}

				v3f First = Ray.lerp(Rec1.t);
				v3f Next = Ray.lerp(Rec2.t);

				v3f Delta = Next - First;


				//f32 delta_t = Rec2.t - Rec1.t;
				//v3f EnterToExit = Ray.lerp(delta_t);

				f32 total_distance_inside = Delta.length();
				f32 distance_to_next_hit = neg_inv_density * log(random_unit());

				if(distance_to_next_hit <= total_distance_inside)
				{
					IntersectRecord.t = Rec1.t + (distance_to_next_hit / total_distance_inside);
					IntersectRecord.Pos = Ray.lerp(IntersectRecord.t);

					// Arbitrary
					IntersectRecord.Normal = v3f(1.0f, 0.0f, 0.0f);
					// Arbitrary
					IntersectRecord.front_face = true;

					IntersectRecord.Material = PhaseFunction;
					

					Result = true;
				}
			}
		}
	}
	return(Result);
}

aabb
medium_constant::bounding_box() const
{
	return(Boundary->bounding_box());
}

#define RAYTRACER_PARTICLES_H
#endif
