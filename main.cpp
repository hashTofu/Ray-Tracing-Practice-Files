#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"

int main(int argc, char* argv[]) {
    unsigned int randseed = 160111;
    if (argc > 1){
        randseed = std::stoi(argv[1]);
    }
    srand(randseed);
    auto main_start_time = std::chrono::high_resolution_clock::now();
    hittable_list world;
    BVHSplitMethod split = BVHSplitMethod::SPATIAL_MEDIAN;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto ground = make_shared<sphere>(point3(0,-1000,0), 1000, ground_material);
    // world.add(ground);

    for (int a = -30; a < 7; a++) {
        for (int b = -25; b < 12; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.15, b + 0.9*random_double());

            if ((center - point3(0, 0.15, 0)).length() > 1.0 && 
                (center - point3(3.25, 0.15, -0.75)).length() > 1.15 &&
                (center - point3(3.25, 0.15, 1.5)).length() > 0.75 &&
                (center - point3(2.5, 0.15, 3.25)).length() > 0.85) {
                shared_ptr<material> sphere_material;
					
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
					// if(choose_mat < 0.4){
						sphere_material = make_shared<lambertian>(albedo);
					// }else{
					// 	sphere_material = make_shared<cel>(albedo, albedo * 0.3, random_double(0, 0.2));
					// }
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, 0.15, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.15, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.15, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    world.add(make_shared<sphere>(point3(2.5, 0.75, 3.25), 0.75, material1));
    auto material4 = make_shared<dielectric>(1.0 / 1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 0.85, material4));
    world.add(make_shared<sphere>(point3(2.5, 0.75, 3.25), 0.65, material4));

    auto material2 = make_shared<cel>(color(0.7, 0.3, 0.7), color(0.247, 0.063, 0.412), 0.3);
    world.add(make_shared<sphere>(point3(3.25, 1, -0.75), 1, material2));

    auto material3 = make_shared<metal>(color(1.0, 0.6, 0.5), 0.01);
    world.add(make_shared<sphere>(point3(-4, 1, 1), 1, material3));
    auto material5 = make_shared<metal>(color(0.5, 0.6, 0.9), 0.01);
    world.add(make_shared<sphere>(point3(3.25, 0.75, 1.5), 0.75, material5));
    auto material8 = make_shared<metal>(color(0.6, 0.333, 0.435), 0.4);
    world.add(make_shared<sphere>(point3(5.8, 0.5, 1.5), 0.5, material8));

    auto material6 = make_shared<lambertian>(color(0.439, 1, 0.333));
    world.add(make_shared<sphere>(point3(5, 0.3, 2.2), 0.3, material6));
    auto material9 = make_shared<lambertian>(color(0.851, 0.961, 0.169));
    world.add(make_shared<sphere>(point3(5, 0.4, 3.3), 0.4, material9));

    auto material7 = make_shared<lambertian>(color(0.333, 0.471, 1));
    world.add(make_shared<sphere>(point3(-6, 1.2, -4), 1.2, material7));

    auto bvh_start_time = std::chrono::high_resolution_clock::now();

    // world.add(ground);
	// world = hittable_list(make_shared<bvh_node>(world));
    world = hittable_list(make_shared<bvh_node>(ground, world, split));
    
    
    auto bvh_stop_time = std::chrono::high_resolution_clock::now();
    auto bvh_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(bvh_stop_time - bvh_start_time);
    std::clog << "BVH construction time: " << bvh_duration.count() << " ns" << std::endl;


    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 128;
    cam.max_depth         = 50;

    cam.vfov     = 50;
    cam.lookfrom = point3(8,2,4);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 1.75;
    cam.focus_dist    = 6;

    cam.render(world);
    auto main_stop_time = std::chrono::high_resolution_clock::now();
    auto main_duration = std::chrono::duration_cast<std::chrono::milliseconds>(main_stop_time - main_start_time);
    std::clog << "Total time: " << main_duration.count() << " ms" << std::endl;
}