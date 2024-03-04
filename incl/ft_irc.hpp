#pragma once

# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <stdlib.h>
# include <signal.h>
# include <arpa/inet.h>
# include <sys/socket.h>

# include <string>
# include <map>
# include <ctime>
# include <vector>
# include <climits>
# include <sstream>
# include <algorithm>
# include <functional>


# include "Server.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "utils.hpp"
# include "replies.hpp"
# include "Channel.hpp"

# define BACKLOG 5
# define HOSTNAME_LEN 32
# define BUFFER 512
# define IRC_ENDLINE "\r\n"
# define POLL_TIMEOUT_MS 3000
# define PING_FREQ 15
# define PING_TIMEOUT 30
# define MAX_NICK_LEN 16
# define LEN_TOKEN 8


# define VALID_CLIENT 0b1000


# define RESET "\033[0m"
# define GREY "\033[90m"
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

// MODE - Change the channel’s mode:
// · i: Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel operators
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// · l: Set/remove the user limit to channel

typedef struct s_channel_modes
{
	bool	mode_i;
	bool	mode_t;
	bool	mode_k;
	bool	mode_o;
	bool	mode_l;
}	t_channel_modes;