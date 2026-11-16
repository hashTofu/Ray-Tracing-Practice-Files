#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <memory>


// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// xorshift128+ : thread_local, so parallel workers never contend and each
// thread gets an independent stream. std::rand() is NOT thread-safe (glibc
// takes a global lock), which serialised every OpenMP worker.
inline uint64_t rng_next() {
	static thread_local uint64_t s[2] = {
		0x9E3779B97F4A7C15ULL ^ 0x2545F4914F6CDD1DULL,
		0xBF58476D1CE4E5B9ULL
	};
	uint64_t x = s[0];
	const uint64_t y = s[1];
	s[0] = y;
	x ^= x << 23;
	s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
	return s[1] + y;
}

inline double random_double(){
	// 53 significant bits -> [0, 1)
	return (rng_next() >> 11) * (1.0 / 9007199254740992.0);
}

inline double random_double(double min, double max){
	return min + (max - min) * random_double();
}

// Common Headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif