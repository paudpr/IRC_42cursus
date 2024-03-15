# pragma once

# include "ft_irc.hpp"

void to_lower(std::string &str);
std::string join_split(std::vector<std::string> array,  size_t start_pos, std::string sep);
std::string generate_token(void);
std::string get_time(void);
std::string get_seconds(std::time_t init_time);
std::string int_to_string(int num);
std::vector<std::string> split(const std::string& s, char delimiter);
std::string unix_time(void);
