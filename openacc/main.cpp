#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main() {
    // World
    hittable_list world;
    
    // Ground - regular lambertian instead of cel
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // Create expanded material palette
    // Lambertian (diffuse) materials with more variety
    auto mat_red_diffuse = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto mat_green_diffuse = make_shared<lambertian>(color(0.3, 0.7, 0.3));
    auto mat_blue_diffuse = make_shared<lambertian>(color(0.3, 0.3, 0.7));
    auto mat_purple_diffuse = make_shared<lambertian>(color(0.5, 0.3, 0.7));
    auto mat_yellow_diffuse = make_shared<lambertian>(color(0.8, 0.8, 0.2));
    
    // Metal materials with varying roughness
    auto mat_gold = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto mat_silver = make_shared<metal>(color(0.8, 0.8, 0.8), 0.2);
    auto mat_copper = make_shared<metal>(color(0.7, 0.4, 0.2), 0.4);
    auto mat_bronze = make_shared<metal>(color(0.8, 0.5, 0.2), 0.3);
    auto mat_chrome = make_shared<metal>(color(0.9, 0.9, 0.9), 0.05);
    
    // Glass materials - more variety
    auto mat_clear_glass = make_shared<dielectric>(1.5);
    auto mat_blue_glass = make_shared<dielectric>(1.3);
    auto mat_green_glass = make_shared<dielectric>(1.4);
    
    // Limited cel-shaded materials (only a few)
    auto mat_red_cel = make_shared<cel>(color(0.9, 0.2, 0.2), color(0.4, 0.0, 0.0), 0.25);
    auto mat_blue_cel = make_shared<cel>(color(0.2, 0.5, 0.9), color(0.0, 0.0, 0.4), 0.3);
    
    // Base row - metal spheres
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat_gold));
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat_silver));
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat_copper));
    
    // First floating row - mixed materials (y=3)
    world.add(make_shared<sphere>(point3(-5, 3, 1), 0.8, mat_red_diffuse));
    world.add(make_shared<sphere>(point3(-3, 3, 1), 0.8, mat_blue_glass));
    world.add(make_shared<sphere>(point3(-1, 3, 1), 0.8, mat_yellow_diffuse));
    world.add(make_shared<sphere>(point3(1, 3, 1), 0.8, mat_green_diffuse));
    world.add(make_shared<sphere>(point3(3, 3, 1), 0.8, mat_bronze));
    world.add(make_shared<sphere>(point3(5, 3, 1), 0.8, mat_purple_diffuse));
    
    // Second floating row - metals and glass (y=5)
    world.add(make_shared<sphere>(point3(-4, 5, 0), 0.7, mat_chrome));
    world.add(make_shared<sphere>(point3(-2, 5, 0), 0.7, mat_green_glass));
    world.add(make_shared<sphere>(point3(0, 5, 0), 0.7, mat_clear_glass));
    world.add(make_shared<sphere>(point3(2, 5, 0), 0.7, mat_blue_glass));
    world.add(make_shared<sphere>(point3(4, 5, 0), 0.7, mat_chrome));
    
    // Highest floating spheres - just a few cel-shaded for contrast (y=7-8)
    world.add(make_shared<sphere>(point3(-3, 7, -1), 0.9, mat_blue_cel));
    world.add(make_shared<sphere>(point3(0, 8, -2), 1.2, mat_silver)); // Large central floating sphere
    world.add(make_shared<sphere>(point3(3, 7, -1), 0.9, mat_red_cel));
    
    // Back row - glass and metal (creating depth)
    world.add(make_shared<sphere>(point3(-3, 1.5, -4), 1.5, mat_clear_glass));
    world.add(make_shared<sphere>(point3(3, 1.5, -4), 1.5, mat_gold));
    
    // Scattered small spheres with various non-cel materials
    int num_small = 20;
    for (int i = 0; i < num_small; i++) {
        double angle = i * 0.3;
        double height = 0.5 + i * 0.2;
        double radius = 0.15 + (i % 3) * 0.05;
        double x = 2.5 * cos(angle);
        double z = 2.5 * sin(angle);
        
        // Mostly non-cel materials
        shared_ptr<material> sphere_material;
        int mat_choice = i % 8;
        if (mat_choice == 0) sphere_material = mat_red_diffuse;
        else if (mat_choice == 1) sphere_material = mat_silver;
        else if (mat_choice == 2) sphere_material = mat_blue_diffuse;
        else if (mat_choice == 3) sphere_material = mat_clear_glass;
        else if (mat_choice == 4) sphere_material = mat_green_diffuse;
        else if (mat_choice == 5) sphere_material = mat_gold;
        else if (mat_choice == 6) sphere_material = mat_bronze;
        else sphere_material = mat_blue_glass;
        
        world.add(make_shared<sphere>(point3(x, height, z), radius, sphere_material));
    }
    
    // Camera setup remains the same
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    
    cam.vfov = 40;
    cam.lookfrom = point3(0, 4, 12);
    cam.lookat = point3(0, 4, 0);
    cam.vup = vec3(0, 1, 0);
    
    cam.defocus_angle = 0.1;
    cam.focus_dist = 12.0;
    
    cam.render(world);
}