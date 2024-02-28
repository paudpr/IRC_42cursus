# include "utils.hpp"

void to_lower(std::string &str)
{
	for (size_t i = 0; i  < str.size(); i++)
		str[i] = std::tolower(str[i]);
}

std::string join_split(std::vector<std::string> array,  size_t start_pos)
{
	std::string joined;
	for (std::vector<std::string>::iterator iter  = array.begin() + start_pos; iter != array.end(); iter++)
	{
		joined += *iter;
		if (iter != array.end() - 1)
			joined += " ";
	}
	return joined;
}

char generate_random(void)
{
	static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	size_t index = rand() % std::strlen(charset);
	return  charset[index];
}

std::string generate_token(void)
{
	srand(static_cast<unsigned int>(time(NULL)));
	std::string token(LEN_TOKEN, '\0');
	std::generate(token.begin(), token.end(), generate_random);
	return token;
}

std::string get_time(void)
{
	time_t tt;
	struct tm *tt_info;
	char  buffer[50];

	time(&tt);
	tt_info = localtime(&tt);
	strftime(buffer, 50, "[%d/%m/%Y][%H:%M:%S]", tt_info);
	std::string str(buffer);
	return str;
}

std::string int_to_string(int num)
{
	std::stringstream ss;
	ss << num;
	std::string str = ss.str();
	return str;
}