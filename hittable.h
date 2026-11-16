#ifndef HITTABLE_H
#define HITTABLE_H

#include "vec3.h"

class material;

class hit_record {
	public:
		point3 p;
		vec3 normal;
		// Raw pointer, not shared_ptr: every hit used to copy the shared_ptr,
		// which means an atomic refcount increment/decrement per bounce. All
		// worker threads hammered the same control blocks, so the cache lines
		// ping-ponged between cores and parallel scaling collapsed.
		// Ownership stays with hittable_list / main(); nothing here outlives it.
		const material* mat = nullptr;
		double t;
		bool front_face;

		void set_face_normal(const ray& r, const vec3& outward_normal) {
			// Sets the hit record normal vector.
			// NOTE: the parameter `outward_normal` is assumed to have unit length.

			front_face = dot(r.direction(), outward_normal) < 0;
			normal = front_face ? outward_normal : -outward_normal;
		}
};

class hittable { 
	public:
		virtual ~hittable() = default;


		virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};


#endif