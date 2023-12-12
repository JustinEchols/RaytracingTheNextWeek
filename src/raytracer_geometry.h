#if !defined(RAYTRACER_GEOMETRY_H)

#include <raytracer_hitable.h>

//
// NOTE(Jusitn): Quad class
//

class quad: public object
{
	public:
		quad(const v3f &_LowerLeft, const v3f &_U, const v3f &_V, material *M);
		virtual void bounding_box_init();
		virtual aabb bounding_box() const;
		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;
		b32 interior_point(f32 alpha, f32 beta, intersect_record &IntersectRecord) const;

		v3f LowerLeft;
		v3f U;
		v3f V;
		material *Material;
		aabb BoundingBox;
		v3f Normal;
		f32 D;

		v3f W;
};

quad::quad(const v3f &_LowerLeft, const v3f &_U, const v3f &_V, material *M)
{
	LowerLeft = _LowerLeft;
	U = _U;
	V = _V;
	Material = M;

	v3f N = cross(U, V);
	Normal = unit_vector(N);
	D = dot(Normal, LowerLeft);

	W = (1.0f / dot(N, N)) * N;

	bounding_box_init();
}

void
quad::bounding_box_init()
{
	BoundingBox = aabb(LowerLeft, LowerLeft + U + V).pad();
}

aabb
quad::bounding_box() const
{
	return(BoundingBox);
}

b32
quad::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;
	f32 t_denom = dot(Normal, Ray.Direction);
	if(ABS(t_denom) >= 1e-8)
	{
		f32 t_num = D - dot(Normal, Ray.Origin);
		f32 t = t_num / t_denom;
		if(RayInterval.contains(t))
		{
			v3f TestPoint = Ray.lerp(t);
			v3f LowerLeftToTestPoint = TestPoint - LowerLeft;
			f32 alpha = dot(W, cross(LowerLeftToTestPoint, V));
			f32 beta = dot(W, cross(U, LowerLeftToTestPoint));
			if(interior_point(alpha, beta, IntersectRecord))
			{
				IntersectRecord.t = t;
				IntersectRecord.Pos = TestPoint;
				IntersectRecord.Material = Material;
				normal_out_set(Ray, Normal, IntersectRecord);

				Result = true;
			}
		}
	}
	return(Result);
}

b32
quad::interior_point(f32 alpha, f32 beta, intersect_record &IntersectRecord) const
{
	b32 Result = true;
	if((alpha < 0) || (alpha > 1) || (beta < 0) || (beta > 1))
	{
		Result = false;
	}
	else
	{
		IntersectRecord.u = alpha;
		IntersectRecord.v = beta;
	}

	return(Result);
}

class box: public object
{
	public:
		box() {}
		box(const v3f &A, const v3f &B, material *Material);
		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;
		virtual void bounding_box_init();
		virtual aabb bounding_box() const;

		object_list Quads;
		v3f P0;
		v3f P1;
		aabb BoundingBox;


};

box::box(const v3f &A, const v3f &B, material *Material)
{
	P0 = A;
	P1 = B;

	v3f Min = v3f(MIN(A.x(), B.x()), MIN(A.y(), B.y()), MIN(A.z(), B.z()));
	v3f Max = v3f(MAX(A.x(), B.x()), MAX(A.y(), B.y()), MAX(A.z(), B.z()));

	v3f dX = v3f(Max.x() - Min.x(), 0.0f, 0.0f);
	v3f dY = v3f(0.0f, Max.y() - Min.y(), 0.0f);
	v3f dZ = v3f(0.0f, 0.0f, Max.z() - Min.z());

	v3f FrontLowerLeft = Min + dZ;
	quad *Front = new quad(FrontLowerLeft, dX, dY, Material);

	v3f RightLowerLeft = Min + dX + dZ;
	quad *Right = new quad(RightLowerLeft, -dZ, dY, Material);

	v3f BackLowerLeft = Min + dX;
	quad *Back = new quad(BackLowerLeft, -dX, dY, Material);

	v3f LeftLowerLeft = Min;
	quad *Left = new quad(LeftLowerLeft, dZ, dY, Material);

	v3f TopLowerLeft = Min + dZ + dY;
	quad *Top = new quad(TopLowerLeft, dX, -dZ, Material);

	v3f BottomLowerLeft = Min;
	quad *Bottom = new quad(BottomLowerLeft, dX, dZ, Material);

	Quads.add(Front);
	Quads.add(Right);
	Quads.add(Back);
	Quads.add(Left);
	Quads.add(Top);
	Quads.add(Bottom);

	bounding_box_init();
}

b32
box::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = Quads.ray_intersect(Ray, RayInterval, IntersectRecord);

	return(Result);
}

void
box::bounding_box_init()
{
	BoundingBox = Quads.BoundingBox;
}

aabb
box::bounding_box() const
{
	return(BoundingBox);
}

#if 0
inline object_list
box(const v3f &A, const v3f &B, material *Material)
{
	object_list Result;

	v3f Min = v3f(MIN(A.x(), B.x()), MIN(A.y(), B.y()), MIN(A.z(), B.z()));
	v3f Max = v3f(MAX(A.x(), B.x()), MAX(A.y(), B.y()), MAX(A.z(), B.z()));

	v3f dX = v3f(Max.x() - Min.x(), 0.0f, 0.0f);
	v3f dY = v3f(0.0f, Max.y() - Min.y(), 0.0f);
	v3f dZ = v3f(0.0f, 0.0f, Max.z() - Min.z());

	v3f FrontLowerLeft = Min + dZ;
	quad *Front = new quad(FrontLowerLeft, dX, dY, Material);

	v3f RightLowerLeft = Min + dX + dZ;
	quad *Right = new quad(RightLowerLeft, -dZ, dY, Material);

	v3f BackLowerLeft = Min + dX;
	quad *Back = new quad(BackLowerLeft, -dX, dY, Material);

	v3f LeftLowerLeft = Min;
	quad *Left = new quad(LeftLowerLeft, dZ, dY, Material);

	v3f TopLowerLeft = Min + dZ + dY;
	quad *Top = new quad(TopLowerLeft, dX, -dZ, Material);

	v3f BottomLowerLeft = Min;
	quad *Bottom = new quad(BottomLowerLeft, dX, dZ, Material);

	Result.add(Front);
	Result.add(Right);
	Result.add(Back);
	Result.add(Left);
	Result.add(Top);
	Result.add(Bottom);

	return(Result);
}
#endif

//
// NOTE(Jusitn): Sphere class
//

class sphere: public object
{
	public:
		sphere() {}
		sphere(v3f C, f32 r, material *M);
		sphere(v3f C0, v3f C1, f32 r, material *M);

		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;

		void normal_out_set(const ray &Ray, const v3f &NormalOut, intersect_record &IntersectRecord) const;
		v3f center_lerp(f32 t) const;
		void uv_coord(const v3f &V, f32 &u, f32 &v) const;

		virtual aabb bounding_box() const;

		v3f Center;
		f32 radius;
		material *Material;
		v3f PosDelta;
		b32 is_moving;

		aabb BoundingBox;

};

sphere::sphere(v3f C, f32 r, material *M)
{
	Center = C;
	radius = r;
	Material = M;
	is_moving = false;

	v3f BoundingDim = v3f(radius, radius, radius);
	BoundingBox = aabb(Center - BoundingDim, Center + BoundingDim);
}

sphere::sphere(v3f C0, v3f C1, f32 r, material *M)
{
	Center = C0;
	radius = r;
	Material = M;
	PosDelta = C1 - C0;
	is_moving = true;

	v3f BoundingDim = v3f(radius, radius, radius);
	aabb BoundingBoxT0 = aabb(C0 - BoundingDim, C0 + BoundingDim);
	aabb BoundingBoxT1 = aabb(C1 - BoundingDim, C1 + BoundingDim);

	BoundingBox = aabb(BoundingBoxT0, BoundingBoxT1);
}

b32
sphere::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;

	v3f SphereCenter = (is_moving ? center_lerp(Ray.time) : Center);
	v3f CenterToOrigin = Ray.Origin - SphereCenter;

	f32 a = dot(Ray.Direction, Ray.Direction);
	f32 b = dot(CenterToOrigin, Ray.Direction);
	f32 c = dot(CenterToOrigin, CenterToOrigin) - SQUARE(radius);

	f32 discriminant = SQUARE(b) - a * c;
	if(discriminant > 0.0f)
	{
		f32 t = ((-b - sqrt(discriminant)) / a);
		if(RayInterval.surrounds(t))
		{
			IntersectRecord.t = t;
			IntersectRecord.Pos = Ray.lerp(IntersectRecord.t);

			v3f NormalOut = (1.0f / radius) * (IntersectRecord.Pos- Center);
			normal_out_set(Ray, NormalOut, IntersectRecord);

			IntersectRecord.Material = Material;

			uv_coord(NormalOut, IntersectRecord.u, IntersectRecord.v);

			Result = true;
		}
		else
		{
			t = ((-b + sqrt(discriminant)) / a);
			if(RayInterval.surrounds(t))
			{
				IntersectRecord.t = t;
				IntersectRecord.Pos = Ray.lerp(IntersectRecord.t);

				v3f NormalOut = (1.0f / radius) * (IntersectRecord.Pos- Center);
				normal_out_set(Ray, NormalOut, IntersectRecord);

				IntersectRecord.Material = Material;

				uv_coord(NormalOut, IntersectRecord.u, IntersectRecord.v);

				Result = true;
			}

		}
	}
	return(Result);
}

void
sphere::normal_out_set(const ray &Ray, const v3f &NormalOut, intersect_record &IntersectRecord) const
{
	object::normal_out_set(Ray, NormalOut, IntersectRecord);
}

v3f
sphere::center_lerp(f32 t) const
{
	v3f Result;

	Result = Center + t * PosDelta;

	return(Result);
}

void
sphere::uv_coord(const v3f &V, f32 &u, f32 &v) const
{
	v2f Result;

	f32 theta = atan2(V.x(), V.z());
	f32 phi = asin(V.y());

	u = theta / (2.0f * PI32);
	v  = (phi + (PI32 / 2.0f)) / PI32;
}


aabb
sphere::bounding_box() const
{
	return(BoundingBox);
}


#define RAYTRACER_GEOMETRY_H
#endif
