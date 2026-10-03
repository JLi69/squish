#pragma once

#include <glm/glm.hpp>

typedef glm::vec4 Color;

namespace colors {
	const Color RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
	const Color GREEN = Color(0.0f, 1.0f, 0.0f, 1.0f);
	const Color BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
	const Color BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
	const Color WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);

	const Color SLIME_GREEN = Color(0.38f, 0.83f, 0.14f, 1.0f);
}
