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
	std::time_t t = std::time(nullptr);

    // Convertir el tiempo a una estructura de tiempo local
    std::tm* tt_info = std::gmtime(&t);

    // Crear un buffer para almacenar la hora formateada
    char buffer[50];

    // Formatear la fecha y hora en el formato deseado
    std::strftime(buffer, sizeof(buffer), "@time=%Y-%m-%dT%H:%M:%S", tt_info);

    // Obtener los milisegundos
    std::clock_t milliseconds = std::clock() % CLOCKS_PER_SEC;

    // Convertir los milisegundos a una cadena
    std::ostringstream milliseconds_stream;
    milliseconds_stream << std::setw(3) << std::setfill('0') << milliseconds;

    // Concatenar los milisegundos al buffer
    std::string result(buffer);
    result += "." + milliseconds_stream.str() + "Z";

    // Convertir a std::string y devolver
    return result;
}

std::string int_to_string(int num)
{
	std::stringstream ss;
	ss << num;
	std::string str = ss.str();
	return str;
}