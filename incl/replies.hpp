#pragma once

// * Returned when a nickname parameter is expected for a command but isn’t given.
# define ERR_NONICKNAMEGIVEN(client) ":ft_irc 431 " + client + " :No nickname given" + IRC_ENDLINE


# define ERR_ALREADYREGISTERED(client) ":ft_irc 462 " + client + " :You may not reregister" + IRC_ENDLINE
# define ERR_PASSWDMISMATCH(client) ":ft_irc 464 " + client + " :Password incorrect" + IRC_ENDLINE


# define RPL_WELCOME(nick, username, hostname) ":ft_irc 001 " + nick + " :Welcome to the ft_irc Internet Relay Chat Network, " + nick + "[!" + username + "@" + hostname + "]" IRC_ENDLINE
# define RPL_YOURHOST(client, servername, version) ":ft_irc 002 " + client + " :Your host is " + servername + ", running version " + version + IRC_ENDLINE
# define RPL_CREATED(client, datetime)  ":ft_irc 003 " + client + " :This server was created " + datetime + IRC_ENDLINE
# define RPL_MYINFO(client, servername, version, availableUserModes, availableChannelModes) ":ft_irc 004 " + client + " " + servername + " " + version + " " + availableUserModes + " " + availableChannelModes + IRC_ENDLINE
# define RPL_ISUPPORT(client) ":ft_irc 005 " + client +  " CHARSET=ascii CHANTYPES=# PREFIX=(o)@ NAMESX=NO :are supported by this server" + IRC_ENDLINE

# define RPL_LUSERCLIENT(client, num) "[251] " + client + ":There are " + num + " users in this server" + IRC_ENDLINE
# define RPL_LUSEROP(client, num ) "[252] "  + client + " "  + num + " :operator(s) online" + IRC_ENDLINE
# define RPL_LUSERUNKNOWN(client) "[253] unknown connections" 

# define RPL_MOTDSTART(client) "[375] " + client + ":-Start Message of the Day" + IRC_ENDLINE
# define RPL_MOTD "[372] :<line  of the motd"
# define RPL_ENDOFMOTD(client) "[376] " + client + " :End of /MOTD command" + IRC_ENDLINE

//redefinir, que coño  que aburrido
# define ERR_NOSUCHNICK(client, nick) ":ft_irc 401 " + client + " " + nick + " :No such nick/channel" + IRC_ENDLINE
# define ERR_NOSUCHSERVER "[402] ERR_NOSUCHSERVER"

# define RPL_CHANGENICK(oldNick, newNick) ":" + oldNick + " NICK " + newNick + IRC_ENDLINE

# define RPL_ENDOFWHOIS "[318] RPL_ENDOFWHOIS"

# define RPL_ENDOFMOTD_MSG ":End of /MOTD command."

// Cannel replies //TODO: redefinir
// * Bad Channel Key
# define ERR_BADCHANNELKEY(client, channel) ":ft_irc 475 " + client + " " + channel + " :Cannot join channel (+k)" + IRC_ENDLINE

//* Invite only channel
# define ERR_INVITEONLYCHAN(client, channel) ":ft_irc 473 " + client + " " + channel + " :Cannot join channel (+i)" + IRC_ENDLINE

// * You're not on that channel
# define ERR_NOTONCHANNEL(client, channel) ":ft_irc 442 " + client + " " + channel + " :You're not on that channel" + IRC_ENDLINE

// * No such channel
# define ERR_NOSUCHCHANNEL(client, channel) ":ft_irc 403 " + client + " " + channel + " :No such channel" + IRC_ENDLINE

// * Too many channels
# define ERR_TOOMANYCHANNELS(client, channel) ":ft_irc 405 " + client + " " + channel + " :You have joined too many channels" + IRC_ENDLINE

// * Banned from channel
# define ERR_BANNEDFROMCHAN(client, channel) ":ft_irc 474 " + client + " " + channel + " :Cannot join channel (+b)" + IRC_ENDLINE

// * Bad Channel Mask
# define ERR_BADCHANMASK(client, channel) ":ft_irc 476 " + client + " " + channel + " :Bad Channel Mask" + IRC_ENDLINE

// * Need more parameters
# define ERR_NEEDMOREPARAMS(client, command) ":ft_irc 461 " + client + " " + command + " :Not enough parameters" + IRC_ENDLINE

// * Channel is full
# define ERR_CHANNELISFULL(client, channel) ":ft_irc 471 " + client + " " + channel + " :Cannot join channel (+l)" + IRC_ENDLINE

// * No topic is set
# define RPL_TOPICWHOTIME(channel, nick, time) ":ft_irc 333 " + channel + " " + nick + " " + time + IRC_ENDLINE

# define RPL_NAMREPLY(client, channel, names) ":ft_irc 353 " + client + " = " + channel + " :" + names + IRC_ENDLINE
# define RPL_ENDOFNAMES(client, channel) ":ft_irc 366 " + channel + " :End of /NAMES list" + IRC_ENDLINE

# define ERR_UNKNOWNMODE(client, modechar) ":ft_irc 472 " + client + " " + modechar + " :is unknown mode char to me" + IRC_ENDLINE
# define ERR_CHANOPRIVSNEEDED(client, channel)":ft_irc 482 " + client + " " + channel + " :You're not channel operator" + IRC_ENDLINE


// * User on channel
# define ERR_USERONCHANNEL(client, nick, channel) ":ft_irc 443 " + client + " " + nick + " " + channel + " :is already on channel" + IRC_ENDLINE
# define ERR_USERNOTINCHANNEL(client, nick, channel) ":ft_irc 441 " + client + " " + nick + " " + channel + " :They aren't on that channel" + IRC_ENDLINE

// * COMMANDS

// INVITE
# define RPL_INVITING(client, nick, channel) ":ft_irc 341 " + client + " " + nick + " " + channel + IRC_ENDLINE

// TOPIC
# define RPL_TOPIC(client, channel, topic) ":ft_irc 332 " + client + " " + channel + " :" + topic + IRC_ENDLINE
# define RPL_NOTICETOPIC(client, channel, topic) ":" + client + " TOPIC " + channel + " " + topic + IRC_ENDLINE

// PRIVMSG
# define RPL_PRIVMSG(client, target, message) ":" + client + " PRIVMSG " + target + " :" + message + IRC_ENDLINE

// MODE
# define RPL_MODE(channel, client, modes) ":" + client + " MODE " + channel + " " + modes + IRC_ENDLINE

//JOIN
# define RPL_JOIN(client, channel) ":" + client + " JOIN " + channel + IRC_ENDLINE

// PART
# define RPL_PART(client, channel) ":" + client + " PART " + channel + IRC_ENDLINE
