#ifndef	UTILS_HPP
# define UTILS_HPP

bool	check_arguments(int ac, char **av);
std::vector<std::string> split(std::string str, std::string delimiter);
std::string joinSplit(std::vector<std::string> split);
std::string getTime(void);

#endif