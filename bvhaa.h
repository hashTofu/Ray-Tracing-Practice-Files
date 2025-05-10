#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
#include <memory>   // <-- Add this line

class bvh_node : public hittable {
  public:

    std::vector<std::shared_ptr<hittable>> node_objects;
    std::vector<std::shared_ptr<hittable>> node_sorted;
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(std::vector<std::shared_ptr<hittable>>& objects, size_t start, size_t end) {
        bbox = aabb::empty;
        for (size_t object_index = start; object_index < end; object_index++)
            bbox = aabb(bbox, objects[object_index]->bounding_box());

        int axis = bbox.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        // Store the objects in this node (unsorted)
        node_objects.assign(objects.begin() + start, objects.begin() + end);

        // Store the sorted array for this node
        node_sorted = node_objects;
        std::sort(node_sorted.begin(), node_sorted.end(), comparator);

        if (object_span == 1) {
            left = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            auto mid = start + object_span / 2;
            // std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end, comparator);
            std::sort(objects.begin(), objects.end(), comparator);
            left = std::make_shared<bvh_node>(objects, start, mid);
            right = std::make_shared<bvh_node>(objects, mid, end);
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = false;
        if (right)
            hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

    void print_tree(const std::string& indent = "", bool is_last_child = true) const {
        std::cout << indent;
        std::cout << (is_last_child ? "\\-- " : "|-- ");
        std::cout << "BVH Node Box: " << bbox << std::endl;

        // Print all objects at this node (unsorted)
        std::cout << indent << "    Objects in node (unsorted):" << std::endl;
        for (const auto& obj : node_objects) {
            std::cout << indent << "      " << obj->bounding_box() << std::endl;
        }

        // Print sorted array for this node
        std::cout << indent << "    Objects in node (sorted):" << std::endl;
        for (const auto& obj : node_sorted) {
            std::cout << indent << "      " << obj->bounding_box() << std::endl;
        }

        // Print the chosen median if this is an internal node with more than 2 objects
        if (node_sorted.size() > 2) {
            size_t median_idx = node_sorted.size() / 2;
            std::cout << indent << "    Chosen median (sorted index " << median_idx << "): "
                        << node_sorted[median_idx]->bounding_box() << std::endl;
        }

        std::string child_indent = indent + (is_last_child ? "    " : "|   ");

        if (left) {
            auto left_bvh = std::dynamic_pointer_cast<bvh_node>(left);
            if (left_bvh) {
                left_bvh->print_tree(child_indent, !right);
            } else {
                std::cout << child_indent;
                std::cout << (!right ? "\\-- " : "|-- ");
                std::cout << "Leaf Box: " << left->bounding_box() << std::endl;
            }
        }

        if (right) {
            auto right_bvh = std::dynamic_pointer_cast<bvh_node>(right);
            if (right_bvh) {
                right_bvh->print_tree(child_indent, true);
            } else {
                std::cout << child_indent;
                std::cout << "\\-- ";
                std::cout << "Leaf Box: " << right->bounding_box() << std::endl;
            }
        }
    }

  private:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb bbox;

    static bool box_compare(
        const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index
    ) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }
};

#endif