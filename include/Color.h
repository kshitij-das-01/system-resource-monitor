#ifndef COLOR_H 
#define COLOR_H

#include <string>

namespace Color {
	// Reset
	inline const std::string RESET = "\033[0m";

	// Regular colors
	inline const std::string RED 		= "\033[31m";
	inline const std::string GREEN  	= "\033[32m";
	inline const std::string YELLOW 	= "\033[33m";
	inline const std::string BLUE 		= "\033[34m";
	inline const std::string MAGENTA 	= "\033[35m";
	inline const std::string CYAN 		= "\033[36m";
	inline const std::string WHITE 		= "\033[37m";
	
	// Bright (Bold) colors
	inline const std::string BRIGHT_RED 	= "\033[1;31m";
	inline const std::string BRIGHT_GREEN 	= "\033[1;32m";
	inline const std::string BRIGHT_YELLOW 	= "\033[1;33m";
	inline const std::string BRIGHT_CYAN	= "\033[1;36m";
	inline const std::string BRIGHT_WHITE	= "\033[1;37m";

	// Styles
	inline const std::string BOLD 			= "\033[1m";
	inline const std::string DIM 			= "\033[2m";
	inline const std::string UNDERLINE 		= "\033[4m";

} // namespace Color

#endif // COLOR_H