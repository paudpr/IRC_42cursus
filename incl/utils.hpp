# pragma once

# include "ft_irc.hpp"

void to_lower(std::string &str);
std::string join_split(std::vector<std::string> array,  size_t start_pos);
std::string generate_token(void);
std::string get_time(void);
std::string int_to_string(int num);