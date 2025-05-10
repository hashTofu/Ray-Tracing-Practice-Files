#ifndef AABB_H
#define AABB_H

class aabb {
  public:
    interval x, y, z;

    aabb() {} // The default AABB is empty, since intervals are empty by default.

    aabb(const interval& x, const interval& y, const interval& z)
      : x(x), y(y), z(z) {}

    aabb(const point3& a, const point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

	aabb(const aabb& box0, const aabb& box1) {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    aabb(const aabb& box, const point3& p) {
        x = interval(fmin(box.x.min, p[0]), fmax(box.x.max, p[0]));
        y = interval(fmin(box.y.min, p[1]), fmax(box.y.max, p[1]));
        z = interval(fmin(box.z.min, p[3]), fmax(box.z.max, p[3]));
    }

    point3 center() const {
        double center_x = (x.min + x.max) * 0.5;
        double center_y = (y.min + y.max) * 0.5;
        double center_z = (z.min + z.max) * 0.5;
        return point3(center_x, center_y, center_z);
    }

    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
	int longest_axis() const {
        // Returns the index of the longest axis of the bounding box.

        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }
    double surface_area() const{
        return fmax(0.0, 2.0*((x.size() * y.size()) + (y.size() * z.size()) + (x.size() * z.size())));
    }
    static const aabb empty, universe;
};
inline std::ostream& operator<<(std::ostream& os, const aabb& box) {
    os << "[(" << box.x.min << "," << box.y.min << "," << box.z.min << ") -> ("
       << box.x.max << "," << box.y.max << "," << box.z.max << ")]";
    return os;
}

const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
#endif