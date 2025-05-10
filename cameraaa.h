#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h" // Assuming includes for color, ray, hittable etc. are here or elsewhere
#include "hittable.h"
#include "color.h"
#include "ray.h"
#include "material.h"

#include <iostream>
#include <vector> // Include vector for storing pixel data
#include <chrono> // For timing (optional but useful)
#include <omp.h>  // Include OpenMP header

class camera {
public:
    double aspect_ratio = 1.0;
    int    image_width = 100;
    int    samples_per_pixel = 10;
    int    max_depth = 10;

    double vfov = 90;
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1); // Ensure lookat is different from lookfrom
    vec3   vup = vec3(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist = 10;

    void render(const hittable& world) {
        initialize();
        std::vector<color> pixel_buffer(image_width * image_height);

        // --- Start Timing ---
        auto start_time = std::chrono::high_resolution_clock::now();

        // --- Parallelize the outer loop (j) ---
        // Threads are created ONCE here.
        // schedule(dynamic, 1) can help with varying scanline render times.
        // schedule(static) might be faster if scanlines take similar time. Experiment!
        #pragma omp parallel for
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                pixel_buffer[j * image_width + i] = pixel_samples_scale * pixel_color;
            }
        }

        auto stop_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time);
        std::clog << "\rRendering complete.                 \n";
        std::clog << "Render time: " << duration.count() << " ms" << std::endl;


        // --- Write the buffer to standard output sequentially ---
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                write_color(std::cout, pixel_buffer[j * image_width + i]);
            }
        }
    }

private:
    int    image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;
    vec3   u, v, w; // Camera frame basis vectors
    vec3   defocus_disk_u;  // Camera defocus disk basis vectors
    vec3   defocus_disk_v;

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

     point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }


    color ray_color(const ray& r, int depth, const hittable& world) const {
		if (depth <= 0)
			return color(0, 0, 0);
		
		hit_record rec;

		if (world.hit(r, interval(0.001, infinity), rec)) {
			ray scattered;
			color attenuation;


			if (rec.mat->scatter(r, rec, attenuation, scattered)){
				hit_record shadow_rec;
				vec3 light_dir = vec3(0, -1, 0);
				ray shadow_r(rec.p + rec.normal * 0.0001, -light_dir);
				bool in_shadow = world.hit(shadow_r, interval(0.001, infinity), shadow_rec);
				if (rec.mat->isCel()){
					if (in_shadow && 
					dot(rec.normal, unit_vector(-r.direction())) >= rec.mat->edgeThreshold()){
					    attenuation *= 0.3;
					}
					return attenuation * rec.mat->rAlbedo(); 
				}
				else{
					if ((shadow_rec.mat) && (shadow_rec.mat->isCel())){
						attenuation *= 0.3;
					}
					return attenuation * ray_color(scattered, depth - 1, world);
				}
				// --- Modification End ---
			}
			// Scatter function returned false (e.g., metal reflection absorbed)
			return color(0, 0, 0);
		}
		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5*(unit_direction.x() + 1.0);
		return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.3, 0.7, 1.0);
	}
};

#endif