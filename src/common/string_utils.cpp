#include "string_utils.hpp"
#include <algorithm>

namespace str_utils {
	std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
		// Copied from Stack Overflow 
		// (https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c)
		std::vector<std::string> tokens;
		std::string scopy = s;
		size_t pos = 0;
    	std::string token;
    	while ((pos = scopy.find(delimiter)) != std::string::npos) {
    	    token = scopy.substr(0, pos);
    	    tokens.push_back(token);
    	    scopy.erase(0, pos + delimiter.length());
    	}
    	tokens.push_back(scopy);
    	return tokens;
	}

	// Copied from Stack Overflow
	// (https://stackoverflow.com/questions/216823/how-can-i-trim-a-stdstring)
	// Trim from the start (in place)
	inline std::string ltrim(const std::string &s) {
		std::string scopy = s;
	    scopy.erase(scopy.begin(), std::find_if(scopy.begin(), scopy.end(), [](unsigned char ch) {
	        return !std::isspace(ch);
	    }));
		return scopy;
	}
	
	// Trim from the end (in place)
	inline std::string rtrim(const std::string &s) {
		std::string scopy = s;
	    scopy.erase(std::find_if(scopy.rbegin(), scopy.rend(), [](unsigned char ch) {
	        return !std::isspace(ch);
	    }).base(), scopy.end());
		return scopy;
	}

	std::string trim(const std::string &s) {
		return ltrim(rtrim(s));
	}
}
