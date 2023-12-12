#if !defined(RAYTRACER_HITABLE_LIST_H)

#include <raytracer_hitable.h>

class object_list: public object
{
	public:
		object_list() {}
		void add(object *Obj);
		void add_list(const object_list &ObjList);
		virtual b32 ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const;

		virtual aabb bounding_box() const;

		std::vector<object *> Objects;
		aabb  BoundingBox;
};


void
object_list::add(object *Obj)
{
	Objects.push_back(Obj);
	BoundingBox = aabb(BoundingBox, Obj->bounding_box());
}


void
object_list::add_list(const object_list &ObjList)
{
	for(u32 index = 0; index < ObjList.Objects.size(); index++)
	{
		object *Obj = ObjList.Objects[index];
		Objects.push_back(Obj);
		BoundingBox = aabb(BoundingBox, Obj->bounding_box());
	}
}

b32
object_list::ray_intersect(const ray &Ray, interval RayInterval, intersect_record &IntersectRecord) const
{
	b32 Result = false;

	intersect_record TempRecord = {};
	f32 t_closest = RayInterval.max;
	for(u32 list_index = 0; list_index < Objects.size(); list_index++)
	{
		if(Objects[list_index]->ray_intersect(Ray, interval(RayInterval.min, t_closest), TempRecord))
		{
			t_closest = TempRecord.t;
			IntersectRecord = TempRecord;
			Result = true;
		}
	}
	return(Result);
}

aabb
object_list::bounding_box() const
{
	return(BoundingBox);
}

#define RAYTRACER_HITABLE_LIST_H
#endif
