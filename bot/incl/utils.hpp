#ifndef	UTILS_HPP
# define UTILS_HPP

struct MemoryStruct {
	char *memory;
	size_t size;
};

bool	check_arguments(int ac, char **av);
std::vector<std::string> split(std::string str, std::string delimiter);
std::string joinSplit(std::vector<std::string> split);
std::string getTime(void);
size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::vector<std::string> splitJson(std::string str);
std::string getByKey(std::vector<std::string> json, std::string key);
int string_to_int(std::string number);
#endif