#if !defined(RAYTRACER_HITABLE_H)

#include <raytracer_ray.h>

#define BBOX_X_AXIS 0
#define BBOX_Y_AXIS 1
#define BBOX_Z_AXIS 2

class aabb
{
	public:
		aabb() {}
		aabb(const interval &IntervalX, const interval &IntervalY, const interval &IntervalZ);
		aabb(const v3f &A, const v3f &B);
		aabb(const aabb &BondingBoxA, const aabb &BoundingBoxB);

		const interval & axis(s32 n) const;
		b32 ray_intersect(const ray &Ray, interval RayInterval) const;
		aabb pad();

		interval X, Y, Z;
};

aabb::aabb(const interval &IntervalX, const interval &IntervalY, const interval &IntervalZ)
{
	X = IntervalX;
	Y = IntervalY;
	Z = IntervalZ;
}

aabb::aabb(const v3f &A, const v3f &B)
{
	X = interval(MIN(A[0], B[0]), MAX(A[0], B[0]));
	Y = interval(MIN(A[1], B[1]), MAX(A[1], B[1]));
	Z = interval(MIN(A[2], B[2]), MAX(A[2], B[2]));
}

aabb::aabb(const aabb &BoundingBoxA, const aabb &BoundingBoxB)
{
	X = interval(BoundingBoxA.X, BoundingBoxB.X);
	Y = interval(BoundingBoxA.Y, BoundingBoxB.Y);
	Z = interval(BoundingBoxA.Z, BoundingBoxB.Z);
}

aabb
operator+(const aabb &BoundingBox, const v3f &Delta)
{
	aabb Result;

	Result.X = BoundingBox.X + Delta.x();
	Result.Y = BoundingBox.Y + Delta.y();
	Result.Z = BoundingBox.Z + Delta.z();

	return(Result);
}

aabb
operator+(const v3f &Delta, const aabb &BoundingBox)
{
	aabb Result;

	Result = BoundingBox + Delta;

	return(Result);
}

const interval &
aabb::axis(s32 n) const
{
	if(n == BBOX_Y_AXIS)
	{
		return(Y);
	}
	else if(n == BBOX_Z_AXIS)
	{
		return(Z);
	}
	else
	{
		return(X);
	}
}

b32
aabb::ray_intersect(const ray &Ray, interval RayInterval) const
{
	b32 Result = true;
	for(s32 comp_idx = 0; comp_idx < 3; comp_idx++)
	{
		f32 t0 = MIN((axis(comp_idx).min - Ray.Origin[comp_idx] / Ray.Direction[comp_idx]),
				     (axis(comp_idx).max - Ray.Origin[comp_idx] / Ray.Direction[comp_idx]));

		f32 t1 = MAX((axis(comp_idx).min - Ray.Origin[comp_idx] / Ray.Direction[comp_idx]),
				     (axis(comp_idx).max - Ray.Origin[comp_idx] / Ray.Direction[comp_idx]));

		RayInterval.min = MAX(t0, RayInterval.min);
		RayInterval.max = MIN(t1, RayInterval.max);

		if(RayInterval.max <= RayInterval.min)
		{
			Result = false;
			break;
		}
	}
	return(Result);
}

aabb
aabb::pad()
{
	aabb Result;

	f32 epsilon = 0.0001f;
	interval IntervalX;
	interval IntervalY;
	interval IntervalZ;

	if(X.size() >= epsilon)
	{
		IntervalX = X;
	}
	else
	{
		IntervalX = X.expand(epsilon);
	}

	if(Y.size() >= epsilon)
	{
		IntervalY = Y;
	}
	else
	{
		IntervalY = Y.expand(epsilon);
	}

	if(Z.size() >= epsilon)
	{
		IntervalZ = Z;
	}
	else
	{
		IntervalZ = Z.expand(epsilon);
	}

	Result = aabb(IntervalX, IntervalY, IntervalZ);

	return(Result);
}

class material;

struct intersect_record
{
	f32 t;
	v3f Pos;
	v3f Normal;
	b32 front_face;
	material *Material;
	f32 u;
	f32 v;
};

class object
{
	public:
		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const = 0;
		void normal_out_set(const ray &Ray, const v3f &NormalOut, intersect_record &IntersectRecord) const;

		virtual aabb bounding_box() const = 0;
};

void
object::normal_out_set(const ray &Ray, const v3f &NormalOut, intersect_record &IntersectRecord) const
{
	b32 is_front_face = false;

	if(dot(Ray.Direction, NormalOut) < 0.0f)
	{
		// NOTE(Justin): Surface normals always point away from the surface. If
		// the dot product with the ray direction and surface normal is
		// negative, the vectors are pointing in the opposite direction. If the
		// vectors are pointing in opposite directions and the normal points
		// out, then the ray is intersecting the surface from the outside.

		is_front_face = true;
	}

	IntersectRecord.front_face = is_front_face;
	IntersectRecord.Normal = (is_front_face ? NormalOut : -(NormalOut));
}

class translate: public object
{
	public:
		translate(object *Obj, const v3f &D) {Object = Obj; BoundingBox = Obj->bounding_box() + D; Delta = D;}

		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;
		virtual aabb bounding_box() const;

		object *Object;
		aabb BoundingBox;
		v3f Delta;

	
};

b32
translate::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;

	ray RayDelta = ray(Ray.Origin - Delta, Ray.Direction, Ray.time);
	if(Object->ray_intersect(RayDelta, RayInterval, IntersectRecord))
	{
		IntersectRecord.Pos += Delta;
		Result = true;
	}
	return(Result);
}

aabb
translate::bounding_box() const
{
	return(BoundingBox);
}

class rotate_y: public object
{
	public:
		rotate_y(object *Obj, f32 angle);

		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;
		virtual aabb bounding_box() const;

		object *Object;
		aabb BoundingBox;
		f32 sin_theta;
		f32 cos_theta;

};

rotate_y::rotate_y(object *Obj, f32 angle) 
{
	Object = Obj;

	BoundingBox = Obj->bounding_box();

	f32 radians = DEG_TO_RAD(angle);

	sin_theta = sin(radians);
	cos_theta = cos(radians);

	v3f RowX = v3f(cos_theta, 0.0f, -sin_theta);
	v3f RowY = v3f(0.0f, 1.0f, 0.0f);
	v3f RowZ = v3f(sin_theta, 0.0f, cos_theta);

	v3f Min = v3f(inf, inf, inf);
	v3f Max = -1.0f * Min;


	for(u32 i = 0; i < 2; i++)
	{
		for(u32 j = 0; j < 2; j++)
		{
			for(u32 k = 0; k < 2; k++)
			{
				f32 x = i * BoundingBox.X.max + (1 - i) * BoundingBox.X.min;
				f32 y = j * BoundingBox.Y.max + (1 - j) * BoundingBox.Y.min;
				f32 z = k * BoundingBox.Z.max + (1 - k) * BoundingBox.Z.min;


				v3f Test = v3f(x, y, z);

				Test[0] = dot(RowX, Test);
				Test[2] = dot(RowZ, Test);

				for(s32 c = 0; c < 3; c++)
				{
					Min[c] = MIN(Min[c], Test[c]);
					Max[c] = MAX(Max[c], Test[c]);
				}
			}
		}
	}
	BoundingBox = aabb(Min, Max);
}

b32
rotate_y::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;

	v3f RowX = v3f(cos_theta, 0.0f, -sin_theta);
	v3f RowY = v3f(0.0f, 1.0f, 0.0f);
	v3f RowZ = v3f(sin_theta, 0.0f, cos_theta);

	v3f Origin = Ray.Origin;
	v3f Direction = Ray.Direction;

	Origin[0] = dot(RowX, Origin);
	Origin[2] = dot(RowZ, Origin);

	Direction[0] = dot(RowX, Direction);
	Direction[2] = dot(RowZ, Direction);

	ray RayRotated(Origin, Direction, Ray.time);

	if(Object->ray_intersect(RayRotated, RayInterval, IntersectRecord))
	{
		v3f WorldP = IntersectRecord.Pos;

		v3f TransposeRowX = v3f(cos_theta, 0.0f, sin_theta);
		v3f TransposeRowZ = v3f(-sin_theta, 0.0f, cos_theta);

		WorldP[0] = dot(TransposeRowX, WorldP);
		WorldP[2] = dot(TransposeRowZ, WorldP);

		v3f WorldNormal = IntersectRecord.Normal;

		WorldNormal[0] = dot(TransposeRowX, WorldNormal);
		WorldNormal[2] = dot(TransposeRowZ, WorldNormal);

		IntersectRecord.Pos = WorldP;
		IntersectRecord.Normal = WorldNormal;

		Result = true;
	}

	return(Result);
}

aabb
rotate_y::bounding_box() const
{
	return(BoundingBox);
}
#define RAYTRACER_HITABLE_H
#endif
