#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>
#include <iterator>

enum class BVHSplitMethod {
    OBJECT_MEDIAN,
    SPATIAL_MEDIAN,
    SWEEP_SAH,   
    BINNING_SAH    
};

// --- SAH Constants ---
constexpr double SAH_TRAVERSAL_COST = 0.3;
constexpr size_t MIN_OBJECTS_IN_LEAF = 2;
constexpr int SAH_NUM_BINS = 16; 


class bvh_node : public hittable {
  public:
    bvh_node(hittable_list list, BVHSplitMethod method = BVHSplitMethod::OBJECT_MEDIAN)
        : bvh_node(list.objects, 0, list.objects.size(), method) {}

    bvh_node(shared_ptr<hittable> ground, hittable_list others, BVHSplitMethod method = BVHSplitMethod::OBJECT_MEDIAN) {
        aabb ground_box = ground ? ground->bounding_box() : aabb::empty;
        left = ground;

        if (others.objects.empty()) {
            right = nullptr;
            bbox = ground_box;
        } else {
            right = make_shared<bvh_node>(others.objects, 0, others.objects.size(), method);
            bbox = aabb(ground_box, right->bounding_box());
        }
    }


    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end, BVHSplitMethod method) {
        bbox = aabb::empty;
        for (size_t object_index = start; object_index < end; object_index++) {
             if (objects[object_index]) {
                 bbox = aabb(bbox, objects[object_index]->bounding_box());
             }
        }

        size_t object_span = end - start;

        if (object_span <= MIN_OBJECTS_IN_LEAF) {
            left = (object_span >= 1 && objects[start]) ? objects[start] : nullptr;
            right = (object_span == 2 && objects[start + 1]) ? objects[start + 1] : nullptr;
            return;
        }

        bool split_successful = false;
        int best_axis = -1;
        size_t best_split_index = start;
        double min_sah_cost = std::numeric_limits<double>::infinity();
        double parent_surface_area = bbox.surface_area();
        double leaf_cost = double(object_span);

        BVHSplitMethod method_to_attempt = method;
        if (method == BVHSplitMethod::BINNING_SAH && object_span < SAH_NUM_BINS) {
            method_to_attempt = BVHSplitMethod::SWEEP_SAH;
        }

        if (method_to_attempt == BVHSplitMethod::SWEEP_SAH) {
            if (parent_surface_area > 1e-9) {
                bool sorted_for_best_axis = false;
                for (int axis = 0; axis < 3; ++axis) {
                    auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;
                    std::sort(objects.begin() + start, objects.begin() + end, comparator);

                    std::vector<aabb> left_bounds(object_span);
                    aabb current_left_bounds = aabb::empty;
                    for (size_t i = 0; i < object_span; ++i) {
                        if (objects[start + i]) current_left_bounds = aabb(current_left_bounds, objects[start + i]->bounding_box());
                        left_bounds[i] = current_left_bounds;
                    }

                    aabb current_right_bounds = aabb::empty;
                    for (size_t i = object_span - 1; i > 0; --i) {
                        if (objects[start + i]) current_right_bounds = aabb(current_right_bounds, objects[start + i]->bounding_box());
                        size_t count_l = i; size_t count_r = object_span - i;
                        double sa_l = left_bounds[i - 1].surface_area(); double sa_r = current_right_bounds.surface_area();
                        double split_cost = SAH_TRAVERSAL_COST + (count_l * sa_l + count_r * sa_r) / parent_surface_area;

                        if (split_cost < min_sah_cost) {
                            min_sah_cost = split_cost; best_axis = axis; best_split_index = start + i;
                            sorted_for_best_axis = true;
                        } else if (axis == best_axis) {
                            sorted_for_best_axis = false;
                        }
                    }
                }

                if (best_axis != -1 && min_sah_cost < leaf_cost) {
                    if (!sorted_for_best_axis) {
                         auto final_comparator = (best_axis == 0) ? box_x_compare : (best_axis == 1) ? box_y_compare : box_z_compare;
                         std::sort(objects.begin() + start, objects.begin() + end, final_comparator);
                    }
                    if (best_split_index > start && best_split_index < end) {
                        left = make_shared<bvh_node>(objects, start, best_split_index, method);
                        right = make_shared<bvh_node>(objects, best_split_index, end, method);
                        split_successful = true;
                    }
                }
            }
        } else if (method_to_attempt == BVHSplitMethod::BINNING_SAH) {
            if (parent_surface_area > 1e-6) {
                aabb centroid_bbox = aabb::empty;
                for (size_t i = start; i < end; ++i) {
                    if(objects[i]) centroid_bbox = aabb(centroid_bbox, objects[i]->bounding_box().center());
                }

                for (int axis = 0; axis < 3; ++axis) {
                    if (bbox.axis_interval(axis).size() < 1e-6 || centroid_bbox.axis_interval(axis).size() < 1e-6) continue;

                    struct Bin { aabb bounds = aabb::empty; size_t count = 0; };
                    Bin bins[SAH_NUM_BINS];
                    double centroid_extent = centroid_bbox.axis_interval(axis).size();
                    double bin_width = (centroid_extent > 1e-9) ? (centroid_extent / SAH_NUM_BINS) : 0.0;
                    double first_bin_min = centroid_bbox.axis_interval(axis).min;

                    for (size_t i = start; i < end; ++i) {
                        if (!objects[i]) continue;
                        aabb obj_bbox = objects[i]->bounding_box();
                        double centroid_coord = obj_bbox.center()[axis];
                        int bin_idx = 0;
                        if (bin_width > 0) {
                            bin_idx = static_cast<int>(((centroid_coord - first_bin_min) / bin_width));
                        }
                        bin_idx = std::clamp(bin_idx, 0, SAH_NUM_BINS - 1);
                        bins[bin_idx].count++;
                        bins[bin_idx].bounds = aabb(bins[bin_idx].bounds, obj_bbox);
                    }

                    aabb left_bounds[SAH_NUM_BINS - 1]; aabb right_bounds[SAH_NUM_BINS - 1];
                    size_t left_counts[SAH_NUM_BINS - 1]; size_t right_counts[SAH_NUM_BINS - 1];
                    aabb current_left_bounds = aabb::empty; size_t current_left_count = 0;
                    aabb current_right_bounds = aabb::empty; size_t current_right_count = 0;
                    for (int i = 0; i < SAH_NUM_BINS - 1; ++i) {
                        current_left_bounds = aabb(current_left_bounds, bins[i].bounds); current_left_count += bins[i].count;
                        left_bounds[i] = current_left_bounds; left_counts[i] = current_left_count;
                        int right_idx = SAH_NUM_BINS - 1 - i;
                        current_right_bounds = aabb(current_right_bounds, bins[right_idx].bounds); current_right_count += bins[right_idx].count;
                        right_bounds[SAH_NUM_BINS - 2 - i] = current_right_bounds; right_counts[SAH_NUM_BINS - 2 - i] = current_right_count;
                    }

                    for (int i = 0; i < SAH_NUM_BINS - 1; ++i) {
                        size_t count_l = left_counts[i]; size_t count_r = right_counts[i];
                        if (count_l == 0 || count_r == 0) continue;
                        double sa_l = left_bounds[i].surface_area(); double sa_r = right_bounds[i].surface_area();
                        double split_cost = SAH_TRAVERSAL_COST + (count_l * sa_l + count_r * sa_r) / parent_surface_area;
                        if (split_cost < min_sah_cost) {
                            min_sah_cost = split_cost; best_axis = axis;
                            best_split_index = start + count_l; // Index where right partition would start
                        }
                    }
                }

                if (best_axis != -1 && min_sah_cost < leaf_cost) {
                    if (best_split_index > start && best_split_index < end) {
                        auto comparator = (best_axis == 0) ? box_x_compare : (best_axis == 1) ? box_y_compare : box_z_compare;
                        std::nth_element(objects.begin() + start, objects.begin() + best_split_index, objects.begin() + end, comparator);

                        left = make_shared<bvh_node>(objects, start, best_split_index, method);
                        right = make_shared<bvh_node>(objects, best_split_index, end, method);
                        split_successful = true;
                    }
                }
            }
        } else if (method_to_attempt == BVHSplitMethod::SPATIAL_MEDIAN) {
            int axis = bbox.longest_axis();
            double mid_coord = (bbox.axis_interval(axis).max + bbox.axis_interval(axis).min) / 2.0;

            auto partition_iter = std::partition(objects.begin() + start, objects.begin() + end,
                [axis, mid_coord](const shared_ptr<hittable>& obj) {
                    if (!obj) return true;
                    return obj->bounding_box().center()[axis] < mid_coord;
                });

            size_t mid_index = std::distance(objects.begin(), partition_iter);

            if (mid_index > start && mid_index < end) {
                left = make_shared<bvh_node>(objects, start, mid_index, method);
                right = make_shared<bvh_node>(objects, mid_index, end, method);
                split_successful = true;
            }
        }
        if (!split_successful) {
            int axis = bbox.longest_axis();
            auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;
            size_t mid = start + object_span / 2;

            std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end, comparator);

            if (mid <= start || mid >= end) {
                 mid = start + std::max(size_t(1), object_span / 2);
                 mid = std::min(mid, end - 1);
                 mid = std::max(mid, start + 1);
            }


            left = make_shared<bvh_node>(objects, start, mid, BVHSplitMethod::OBJECT_MEDIAN);
            right = make_shared<bvh_node>(objects, mid, end, BVHSplitMethod::OBJECT_MEDIAN);
        }
    }


    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left = left && left->hit(r, ray_t, rec);
        interval right_interval = hit_left ? interval(ray_t.min, rec.t) : ray_t;
        bool hit_right = right && right->hit(r, right_interval, rec);

        return hit_left || hit_right;
    }


    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    static bool box_compare(
        const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index
    ) {
        if (!a && !b) return false;
        if (!a) return true;
        if (!b) return false;

        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }


    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 0); }
    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 1); }
    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) { return box_compare(a, b, 2); }
};



#endif