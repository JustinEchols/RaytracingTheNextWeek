#if !defined(RAYTRACER_BVH)

#include <algorithm>

class bvh_node: public object 
{
public:

object *Left;
object *Right;

aabb BoundingBox;

bvh_node(const std::vector<object *> &Objects, size_t start, size_t end)
{
	std::vector<object *> Objs = Objects;

	s32 axis = s32_rand_interval(0, 2);

	static b32 (*compare_func)(const object *ObjA, const object *ObjB) = NULL;
	if(axis == BBOX_X_AXIS)
	{
		compare_func = &bvh_node::bounding_box_x_compare;

	}
	else if(axis == BBOX_Y_AXIS)
	{
		compare_func = &bvh_node::bounding_box_y_compare;
	}
	else
	{
		compare_func = &bvh_node::bounding_box_z_compare;
	}

	size_t span = end - start;
	if(span == 1)
	{
		Left = Right = Objs[start];
	}
	else if(span == 2)
	{
		if((compare_func)(Objs[start], Objs[start + 1]))
		{
			Left = Objs[start];
			Right = Objs[start + 1];
		}
		else
		{
			Left = Objs[start + 1];
			Right = Objs[start];
		}
	}
	else
	{
		std::sort(Objs.begin() + start, Objs.begin() + end, compare_func);

		size_t mid = start + (span / 2);
		Left = new bvh_node(Objs, start, mid);
		Right = new bvh_node(Objs, mid, end);
	}

	BoundingBox = aabb(Left->bounding_box(), Right->bounding_box());
}

virtual b32
ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;
	if(BoundingBox.ray_intersect(Ray, RayInterval))
	{
		b32 intersect_left = Left->ray_intersect(Ray, RayInterval, IntersectRecord);
		f32 t;
		if(intersect_left == IntersectRecord.t)
		{
			t = IntersectRecord.t;
		}
		else
		{
			t = RayInterval.max;
		}
		b32 intersect_right = Right->ray_intersect(Ray, interval(RayInterval.min, t), IntersectRecord);

		Result = ((intersect_left) || (intersect_right));
	}
	return(Result);
}


static b32
bvh_node::bounding_box_compare(const object *ObjA, const object *ObjB, s32 axis_index)
{
	b32 Result;

	interval AxisA = ObjA->bounding_box().axis(axis_index);
	interval AxisB = ObjB->bounding_box().axis(axis_index);

	Result = (AxisA.min < AxisB.min);

	return(Result);
}

static b32
bvh_node::bounding_box_x_compare(const object *ObjA, const object *ObjB)
{
	b32 Result = bounding_box_compare(ObjA, ObjB, BBOX_X_AXIS);

	return(Result);
}

static b32
bvh_node::bounding_box_y_compare(const object *ObjA, const object *ObjB)
{
	b32 Result = bounding_box_compare(ObjA, ObjB, BBOX_Y_AXIS);

	return(Result);
}

static b32
bvh_node::bounding_box_z_compare(const object *ObjA, const object *ObjB)
{
	b32 Result = bounding_box_compare(ObjA, ObjB, BBOX_Z_AXIS);

	return(Result);
}

virtual aabb
bvh_node::bounding_box() const
{
	return(BoundingBox);
}

};


#define RAYTRACER_BVH
#endif
