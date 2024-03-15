#ifndef IRC_FORMAT_HPP
#define IRC_FORMAT_HPP

# define RESET "\x0F"

// FORMAT
# define BOLD "\x02"
# define ITALIC "\x1D"
# define UNDERLINE "\x1F"
# define REVERSE "\x16"

// COLORS
# define WHITE "\\x0300"
# define BLACK "\\x0301"
# define BLUE "\\x0302"
# define GREEN "\\x0303"
# define RED "\x0304"
# define BROWN "\\x0305"
# define PURPLE "\\x0306"
# define ORANGE "\\x0307"
# define YELLOW "\\x0308"
# define LIGHT_GREEN "\\x0309"
# define TEAL "\\x0310"
# define LIGHT_CYAN "\\x0311"
# define LIGHT_BLUE "\\x0312"
# define PINK "\\x0313"
# define GREY "\\x0314"
# define LIGHT_GREY "\\x0315"

# define COLOR(color, msg) color + msg + RESET
# define FORMAT(format, msg) format + msg + RESET
# define COLOR_FORMAT(color, format, msg) format + color + msg + RESET

#endif