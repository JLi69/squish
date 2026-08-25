#include "random_utils.hpp"
#include <cstdlib>

float randf() {
	return float(rand()) / float(RAND_MAX);
}

float randf_range(float lower, float upper) {
	return lower + (upper - lower) * randf();
}
