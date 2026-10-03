#pragma once

#include <vector>
#include <string>

namespace str_utils {
	std::vector<std::string> split(const std::string &s, const std::string &delimiter);
	// Trim from the start
	inline std::string ltrim(const std::string &s);	
	// Trim from the end
	inline std::string rtrim(const std::string &s);
	// Trim from both ends
	std::string trim(const std::string &s);
}
