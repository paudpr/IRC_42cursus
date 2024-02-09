#pragma once

# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>

# include <string>
# include <map>
# include <ctime>
# include <vector>
# include <climits>
# include <iostream>
# include <sstream>
# include <algorithm>
# include <functional>


# include "Server.hpp"
# include "User.hpp"
# include "Message.hpp"
# include "utils.hpp"
# include "Replies.hpp"

# define BACKLOG 5
# define HOSTNAME_LEN 32
# define BUFFER 512
# define IRC_ENDLINE "\r\n"
# define POLL_TIMEOUT_MS 3000
# define PINGPONG_FREQ_S 120
# define PINGPONG_TIMEOUT_S 30
# define MAX_NICK_LEN 9


# define VALID_USER 0b1000


# define RESET "\033[0m"
# define GREEN "\033[0;32m"
# define RED "\033[0;31m"
# define BLUE "\033[0;34m"
# define YELLOW "\033[33m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define ORANGE "\033[91m"
# define PINK "\033[95m"
# define LIGHT_GREEN "\033[92m"
# define LIGHT_BLUE "\033[94m"
# define LIGHT_CYAN "\033[96m"
