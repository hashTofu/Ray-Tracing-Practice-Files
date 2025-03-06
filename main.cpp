#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"



int main(){
	// World
	hittable_list world;
	
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    // for (int a = -11; a < 11; a++) {
    //     for (int b = -11; b < 11; b++) {
    //         auto choose_mat = random_double();
    //         point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

    //         if ((center - point3(4, 0.2, 0)).length() > 0.9) {
    //             shared_ptr<material> sphere_material;

    //             if (choose_mat < 0.8) {
    //                 // diffuse
    //                 auto albedo = color::random() * color::random();
    //                 sphere_material = make_shared<lambertian>(albedo);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             } else if (choose_mat < 0.95) {
    //                 // metal
    //                 auto albedo = color::random(0.5, 1);
    //                 auto fuzz = random_double(0, 0.5);
    //                 sphere_material = make_shared<metal>(albedo, fuzz);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             } else {
    //                 // glass
    //                 sphere_material = make_shared<dielectric>(1.5);
    //                 world.add(make_shared<sphere>(center, 0.2, sphere_material));
    //             }
    //         }
    //     }
    // }

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

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,3,5);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

	cam.render(world);
}