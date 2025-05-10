// world1
    hittable_list world1;
    
    // Ground - stylized cel ground
    auto ground_material = make_shared<cel>(color(0.4, 0.5, 0.4), color(0.2, 0.3, 0.2), 0.1);
    world1.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // Create expanded material palette
    // Lambertian (diffuse) materials
    auto mat_red_diffuse = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto mat_green_diffuse = make_shared<lambertian>(color(0.3, 0.7, 0.3));
    auto mat_blue_diffuse = make_shared<lambertian>(color(0.3, 0.3, 0.7));
    
    // Metal materials with varying roughness
    auto mat_gold = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto mat_silver = make_shared<metal>(color(0.8, 0.8, 0.8), 0.2);
    auto mat_copper = make_shared<metal>(color(0.7, 0.4, 0.2), 0.4);
    
    // Glass materials
    auto mat_glass = make_shared<dielectric>(1.5);
    auto mat_tinted_glass = make_shared<dielectric>(1.3);
    
    // Enhanced cel-shaded materials with varying thresholds
    auto mat_red_cel = make_shared<cel>(color(0.9, 0.2, 0.2), color(0.4, 0.0, 0.0), 0.25);
    auto mat_blue_cel = make_shared<cel>(color(0.2, 0.5, 0.9), color(0.0, 0.0, 0.4), 0.3);
    auto mat_green_cel = make_shared<cel>(color(0.2, 0.9, 0.2), color(0.0, 0.4, 0.0), 0.35);
    auto mat_yellow_cel = make_shared<cel>(color(0.9, 0.9, 0.2), color(0.4, 0.4, 0.0), 0.2);
    auto mat_purple_cel = make_shared<cel>(color(0.8, 0.2, 0.8), color(0.3, 0.0, 0.3), 0.4);
    auto mat_cyan_cel = make_shared<cel>(color(0.2, 0.8, 0.8), color(0.0, 0.3, 0.3), 0.45);
    auto mat_orange_cel = make_shared<cel>(color(0.9, 0.5, 0.1), color(0.4, 0.2, 0.0), 0.3);
    auto mat_pink_cel = make_shared<cel>(color(0.9, 0.7, 0.8), color(0.4, 0.2, 0.3), 0.35);
    
    // Base row - metal spheres
    world1.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat_gold));
    world1.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat_silver));
    world1.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat_copper));
    
    // First floating row - cel-shaded (y=3)
    world1.add(make_shared<sphere>(point3(-5, 3, 1), 0.8, mat_red_cel));
    world1.add(make_shared<sphere>(point3(-3, 3, 1), 0.8, mat_orange_cel));
    world1.add(make_shared<sphere>(point3(-1, 3, 1), 0.8, mat_yellow_cel));
    world1.add(make_shared<sphere>(point3(1, 3, 1), 0.8, mat_green_cel));
    world1.add(make_shared<sphere>(point3(3, 3, 1), 0.8, mat_blue_cel));
    world1.add(make_shared<sphere>(point3(5, 3, 1), 0.8, mat_purple_cel));
    
    // Second floating row - higher (y=5)
    world1.add(make_shared<sphere>(point3(-4, 5, 0), 0.7, mat_cyan_cel));
    world1.add(make_shared<sphere>(point3(-2, 5, 0), 0.7, mat_pink_cel));
    world1.add(make_shared<sphere>(point3(0, 5, 0), 0.7, mat_glass));
    world1.add(make_shared<sphere>(point3(2, 5, 0), 0.7, mat_tinted_glass));
    world1.add(make_shared<sphere>(point3(4, 5, 0), 0.7, mat_green_cel));
    
    // Highest floating spheres (y=7-8)
    world1.add(make_shared<sphere>(point3(-3, 7, -1), 0.9, mat_blue_cel));
    world1.add(make_shared<sphere>(point3(0, 8, -2), 1.2, mat_silver)); // Large central floating sphere
    world1.add(make_shared<sphere>(point3(3, 7, -1), 0.9, mat_red_cel));
    
    // Back row - mixed materials (creating depth)
    world1.add(make_shared<sphere>(point3(-3, 1.5, -4), 1.5, mat_glass));
    world1.add(make_shared<sphere>(point3(3, 1.5, -4), 1.5, mat_purple_cel));
    
    // Scattered small cel-shaded spheres (for visual complexity)
    // Using a spiral pattern for the smaller spheres
    int num_small = 20;
    for (int i = 0; i < num_small; i++) {
        double angle = i * 0.3;
        double height = 0.5 + i * 0.2;
        double radius = 0.15 + (i % 3) * 0.05;
        double x = 2.5 * cos(angle);
        double z = 2.5 * sin(angle);
        
        // Alternate materials
        shared_ptr<material> sphere_material;
        int mat_choice = i % 5;
        if (mat_choice == 0) sphere_material = mat_red_cel;
        else if (mat_choice == 1) sphere_material = mat_yellow_cel;
        else if (mat_choice == 2) sphere_material = mat_blue_cel;
        else if (mat_choice == 3) sphere_material = mat_green_cel;
        else sphere_material = mat_purple_cel;
        
        world1.add(make_shared<sphere>(point3(x, height, z), radius, sphere_material));
    }