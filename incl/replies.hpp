#pragma once

# define ERR_NONICKNAMEGIVEN(client) ": [431] " + client + " :No nickname given" + IRC_ENDLINE
# define ERR_NEEDMOREPARAMS(client) ": [461] " + client + " USER :Not enough parameters" + IRC_ENDLINE
# define ERR_ALREADYREGISTERED(client) ": [462] " + client + " :You may not reregister" + IRC_ENDLINE
# define ERR_PASSWDMISMATCH(client) ": [464] " + client + " :Password incorrect" + IRC_ENDLINE


# define RPL_WELCOME(client, fullname) "[001] " + client + ":Welcome to the FT_IRC Network " + fullname + IRC_ENDLINE
# define RPL_YOURHOST(client, servername, version) "[002] " + client + ":Your host is " + servername + ", running version " + version + IRC_ENDLINE
# define RPL_CREATED(client, datetime)  "[003] "  + client + ":This server was created " + datetime + IRC_ENDLINE
# define RPL_MYINFO(client) "[004] " + client + ": reply my info con  toda la  mierda necesaria"
# define RPL_ISUPPORT(client) "[005] " + client + ": <1-13> tokens are supported by this server" + IRC_ENDLINE

# define RPL_LUSERCLIENT(client, num) "[251] " + client + ":There are " + num + " users in this server" + IRC_ENDLINE
# define RPL_LUSEROP(client, num ) "[252] "  + client + " "  + num + " :operator(s) online" + IRC_ENDLINE
# define RPL_LUSERUNKNOWN(client) "[253] unknown connections" 

# define RPL_MOTDSTART(client) "[375] " + client + ":-Start Message of the Day" + IRC_ENDLINE
# define RPL_MOTD "[372] :<line  of the motd"
# define RPL_ENDOFMOTD(client) "[376] " + client + " :End of /MOTD command" + IRC_ENDLINE

//redefinir, que coño  que aburrido
# define ERR_NOSUCHNICK "[401] ERR_NOSUCHNICK"
# define ERR_NOSUCHSERVER "[402] ERR_NOSUCHSERVER"
// # define ERR_NONICKNAMEGIVEN "[431] ERR_NONICKNAMEGIVEN"

# define RPL_CHANGENICK(oldNick, newNick) ":" + oldNick + " NICK " + newNick + IRC_ENDLINE

# define RPL_ENDOFWHOIS "[318] RPL_ENDOFWHOIS"

# define RPL_ENDOFMOTD_MSG ":End of /MOTD command."