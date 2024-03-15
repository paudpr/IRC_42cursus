#ifndef IRC_BOT_HPP
#define IRC_BOT_HPP

# include <iostream>
# include <iomanip>
# include <unistd.h>
# include <poll.h>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <stdlib.h>
# include <signal.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <stdio.h>
#include <sys/errno.h>

# include <string>
# include <map>
# include <ctime>
# include <vector>
# include <climits>
# include <sstream>
# include <algorithm>
# include <functional>

#include "Bot.hpp"
#include "utils.hpp"
#include "colors.hpp"
#include "irc_format.hpp"

# define POLL_TIMEOUT_MS 3000

// Replies

# define RPL_INVITING "341"

#endif