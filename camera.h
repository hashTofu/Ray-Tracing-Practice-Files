#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include "omp.h"
// #include "vec3.h"
// #include "color.h"
// #include "ray.h"

class camera {
	public:
		double	aspect_ratio 		= 1.0;
		int		image_width 		= 100;
		int		samples_per_pixel 	= 10;
		int 	max_depth			= 10;

		double vfov 	= 90;
		point3 lookfrom = point3(0, 0, 0);
		point3 lookat	= point3(0, 0, 0);
		vec3   vup		= vec3(0, 1, 0);

		double defocus_angle = 0;  // Variation angle of rays through each pixel
    	double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

		void render(const hittable& world){
			initialize();

			// render

			std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

			for(int j = 0; j < image_height; ++j){
				std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
				for(int i = 0; i < image_width; ++i){
					color pixel_color(0, 0, 0);
					
					#pragma omp parallel
					{
						color thread_color(0, 0, 0);
						#pragma omp for
						for(int sample = 0; sample < samples_per_pixel; ++sample) {
							ray r = get_ray(i, j);
							thread_color += ray_color(r, max_depth, world);
						}
						#pragma omp critical
							pixel_color += thread_color;
					}
					write_color(std::cout, pixel_samples_scale * pixel_color);
				}
			}
			std::clog << "\rDone.                 \n";
		}

	private:
	    int    image_height;   		// Rendered image height
		double pixel_samples_scale;	// Color scale factor for a sum of pixel samples
		point3 center;         		// Camera center
		point3 pixel00_loc;    		// Location of pixel 0, 0
		vec3   pixel_delta_u;  		// Offset to pixel to the right
		vec3   pixel_delta_v;  		// Offset to pixel below
		vec3   u, v, w;				// Camera frame basis vectors
		vec3   defocus_disk_u;		// Defocus disk horizontal radius
		vec3   defocus_disk_v;		// Defocus disk vertical radius

		void initialize() {
			image_height = int(image_width / aspect_ratio);
			image_height = (image_height < 1) ? 1 : image_height;

			pixel_samples_scale = 1.0 / samples_per_pixel;

			center = lookfrom;
			
			// Camera
			auto theta = degrees_to_radians(vfov);
			auto h = std::tan(theta / 2);
			auto viewport_height = 2 * h * focus_dist;
			auto viewport_width = viewport_height * (double(image_width)/image_height);

			w = unit_vector(lookfrom - lookat);
			u = unit_vector(cross(vup, w));
			v = cross(w, u);

			// Calculate the vectors across the horizontal and down the vertical viewport edges.
			vec3 viewport_u = viewport_width * u;
			vec3 viewport_v = viewport_height * -v;

			pixel_delta_u = viewport_u / image_width;
			pixel_delta_v = viewport_v / image_height;

			// upper left pixel

			auto viewport_upper_left = center
									- (focus_dist * w)
									- viewport_u / 2 - viewport_v / 2;
			pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

			auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
			defocus_disk_u = u * defocus_radius;
			defocus_disk_v = v * defocus_radius;
		}

		ray get_ray(int i, int j) const {
			// Construct a camera ray originating from the origin and directed at randomly sampled
        	// point around the pixel location i, j.


			auto offset = sample_square();
			auto pixel_sample = pixel00_loc
								+ ((i + offset.x()) * pixel_delta_u)
								+ ((j + offset.y()) * pixel_delta_v);

			auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
			auto ray_direction = pixel_sample - ray_origin;

			return ray(ray_origin, ray_direction);
		}

		vec3 sample_square() const {
			// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
			return vec3(random_double() - 0.5, random_double() - 0.5, 0);
		}
		
		point3 defocus_disk_sample() const {
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
					vec3 light_dir = vec3(0, -1, 0);
					ray shadow_r(rec.p + rec.normal * 0.0001, -light_dir);
					hit_record shadow_rec;
					// if shadow ray collided any hittable
					bool in_shadow = world.hit(shadow_r, interval(0.001, infinity), shadow_rec);
					if(in_shadow && (rec.mat->isCel() || shadow_rec.mat->isCel())){
						if(dot(rec.normal, unit_vector(-r.direction())) >= 0.3){
							attenuation *= 0.3;
						}
					}
					return attenuation * ray_color(scattered, depth-1, world);
				}
				return color(0, 0, 0);
			}
			vec3 unit_direction = unit_vector(r.direction());
			auto a = 0.5*(unit_direction.x() + 1.0);
			return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.3, 0.7, 1.0);
		}
};

#endif