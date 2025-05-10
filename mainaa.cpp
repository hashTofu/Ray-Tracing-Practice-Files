#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvhaa.h"
#include <iostream> // Include for std::cout/clog

int main() {
    srand(160110420); // Use a fixed seed for reproducible results if desired
    auto main_start_time = std::chrono::high_resolution_clock::now();
    hittable_list world_objects; // Rename to avoid confusion with the final world BVH

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world_objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    world_objects.add(make_shared<sphere>(point3(-1.7, 0.2, -2.3), 0.2, make_shared<lambertian>(color(0.8, 0.3, 0.3))));
    world_objects.add(make_shared<sphere>(point3(-2.5, 0.2,  1.6), 0.2, make_shared<lambertian>(color(0.3, 0.8, 0.3))));
    world_objects.add(make_shared<sphere>(point3( 1.8, 0.2, -2.7), 0.2, make_shared<lambertian>(color(0.3, 0.3, 0.8))));
    world_objects.add(make_shared<sphere>(point3( 2.4, 0.2,  2.1), 0.2, make_shared<metal>(color(0.8, 0.8, 0.8), 0.1)));
    world_objects.add(make_shared<sphere>(point3( 0.5, 0.2,  2.5), 0.2, make_shared<metal>(color(0.8, 0.6, 0.2), 0.0)));
    world_objects.add(make_shared<sphere>(point3(-0.6, 0.2, -1.8), 0.2, make_shared<dielectric>(1.5)));
    world_objects.add(make_shared<sphere>(point3(-2.2, 0.2,  0.7), 0.2, make_shared<dielectric>(1.3)));
    world_objects.add(make_shared<sphere>(point3( 2.1, 0.2, -0.4), 0.2, make_shared<lambertian>(color(0.2, 0.6, 0.8))));
    // std::clog << "Generated " << sphere_count << " small spheres." << std::endl;
    // --- End smaller scene ---


    auto material1 = make_shared<dielectric>(1.5);
    world_objects.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world_objects.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world_objects.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    std::clog << "Total objects before BVH: " << world_objects.objects.size() << std::endl;

    auto bvh_start_time = std::chrono::high_resolution_clock::now();

    // Construct the BVH
    auto world_bvh_root = make_shared<bvh_node>(world_objects);

    auto bvh_stop_time = std::chrono::high_resolution_clock::now();
    auto bvh_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(bvh_stop_time - bvh_start_time);
    std::clog << "BVH construction time: " << bvh_duration.count() << " ns" << std::endl;

    // --- Print the BVH structure ---
    std::cout << "\n--- BVH Structure Visualization ---" << std::endl;
    world_bvh_root->print_tree();
    std::cout << "---------------------------------" << std::endl;
    // --- End printing ---

    // Create the final world object containing just the BVH root
    hittable_list final_world;
    final_world.add(world_bvh_root);


    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400; // Keep image size reasonable for testing
    cam.samples_per_pixel = 10; // Reduce samples for faster test render
    cam.max_depth         = 10; // Reduce depth for faster test render

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(final_world); // Render using the BVH

    auto main_stop_time = std::chrono::high_resolution_clock::now();
    auto main_duration = std::chrono::duration_cast<std::chrono::milliseconds>(main_stop_time - main_start_time);
    std::clog << "Total time: " << main_duration.count() << " ms" << std::endl;

    return 0; // Added return statement
}