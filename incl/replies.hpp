#pragma once

# define ERR_NONICKNAMEGIVEN(client) "431 " + client + " :No nickname given" + IRC_ENDLINE
# define ERR_NEEDMOREPARAMS(command, client) "461 " + command + " " + client + ":Not enough parameters" + IRC_ENDLINE
# define ERR_ALREADYREGISTERED(command, client) "462 " + command + " " + client + ":You may not reregister" + IRC_ENDLINE
# define ERR_PASSWDMISMATCH(command) "464 " + command + " :Password incorrect" + IRC_ENDLINE

# define RPL_WELCOME(client) "001 " + client + " :Welcome to ft_IRC Network!" + IRC_ENDLINE
# define RPL_YOURHOST(client, servername, version) "002 " + client + " :Your host is " + servername + ", running version " + version + "" + IRC_ENDLINE
# define RPL_CREATED(client, date) "003 " + client + " :This server was created " + date + "" + IRC_ENDLINE
# define RPL_MYINFO(client) "004 " + client + " :Reply my info con  toda la  mierda necesaria" + IRC_ENDLINE
# define RPL_ISUPPORT(client) "005 " + client + " :MOTD File is: motd" + IRC_ENDLINE

# define RPL_MOTDSTART(client) "375 " + client + " :Start Message of the Day" + IRC_ENDLINE
# define RPL_MOTD(client) "372 " + client + " :<line  of the motd>" + IRC_ENDLINE
# define RPL_ENDOFMOTD(client) "376 " + client + " :End of /MOTD command" + IRC_ENDLINE

# define RPL_LUSERCLIENT(client, num) "251 " + client + " :There are " + num + " users in this server" + IRC_ENDLINE
# define RPL_LUSEROP(client, num ) "252 "  + client + " " + num + " :operator(s) online" + IRC_ENDLINE
# define RPL_LUSERUNKNOWN(client) "253 unknown connections" 

# define ERR_ERRONEUSNICKNAME(command) "432 " + command + " :Erroneus nickname" + IRC_ENDLINE
# define ERR_NICKNAMEINUSE(command) "433 " + command + " :Nickname already in use" + IRC_ENDLINE

# define RPL_CHANGENICK(oldNick, newNick) ":" + oldNick + " NICK " + newNick + IRC_ENDLINE

# define RPL_ENDOFWHOIS "318 RPL_ENDOFWHOIS"

# define RPL_NONE(command, message) "300 " + command + " :"  + message + IRC_ENDLINE

# define RPL_QUIT(command, message) command + " " + message + IRC_ENDLINE