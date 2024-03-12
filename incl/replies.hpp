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

// LUSER
# define RPL_LUSERCLIENT(client, num) ":ft_irc 251 " + client + " :There are " + num + " users and 0 services on 1 servers" + IRC_ENDLINE
# define RPL_LUSEROP(client, num ) ":ft_irc 252 " + client + " " + num + " :operator(s) online" + IRC_ENDLINE
# define RPL_LUSERUNKNOWN(client) ":ft_irc 253 " + client + " 0 :unknown connection(s)" + IRC_ENDLINE
# define RPL_LUSERCHANNELS(client, num) ":ft_irc 254 " + client + " " + num + " :channels formed" + IRC_ENDLINE
# define RPL_LUSERME(client, num, num2) ":ft_irc 255 " + client + " :I have " + num + " clients and " + num2 + " servers" + IRC_ENDLINE
# define RPL_lOCALUSERS(client, num, max) ":ft_irc 265 " + client + " :Current local users: " + num + ", max: " + max + IRC_ENDLINE
# define RPL_GLOBALUSERS(client, num, max) ":ft_irc 266 " + client + " :Current global users: " + num + ", max: " + max + IRC_ENDLINE

# define RPL_MOTDSTART(client) "[375] " + client + ":-Start Message of the Day" + IRC_ENDLINE
# define RPL_MOTD "[372] :<line  of the motd"
# define RPL_ENDOFMOTD(client) "[376] " + client + " :End of /MOTD command" + IRC_ENDLINE

// * No such nick/channel
# define ERR_NOSUCHNICK(client, nick) ":ft_irc 401 " + client + " " + nick + " :No such nick/channel" + IRC_ENDLINE

// * No such server
# define ERR_NOSUCHSERVER "[402] ERR_NOSUCHSERVER"

// * No such channel
# define RPL_CHANGENICK(old_nick, new_nick) ":" + old_nick + " NICK " + new_nick + IRC_ENDLINE

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

// * Channel created
# define RPL_NAMREPLY(client, channel, names) ":ft_irc 353 " + client + " = " + channel + " :" + names + IRC_ENDLINE

// * End of /NAMES list
# define RPL_ENDOFNAMES(client, channel) ":ft_irc 366 " + channel + " :End of /NAMES list" + IRC_ENDLINE

// * Uknonwn mode
# define ERR_UNKNOWNMODE(client, modechar) ":ft_irc 472 " + client + " " + modechar + " :is unknown mode char to me" + IRC_ENDLINE

// * You're not channel operator
# define ERR_CHANOPRIVSNEEDED(client, channel)":ft_irc 482 " + client + " " + channel + " :You're not channel operator" + IRC_ENDLINE

// * User on channel
# define ERR_USERONCHANNEL(client, nick, channel) ":ft_irc 443 " + client + " " + nick + " " + channel + " :is already on channel" + IRC_ENDLINE

// * They aren't on that channel
# define ERR_USERNOTINCHANNEL(client, nick, channel) ":ft_irc 441 " + client + " " + nick + " " + channel + " :They aren't on that channel" + IRC_ENDLINE

// ERR_NOTOPLEVEL (413)
// ERR_WILDTOPLEVEL (414)
// * cannot send to channel
# define ERR_CANNOTSENDTOCHAN(client, channel) ":ft_irc 404 " + client + " " + channel + " :Cannot send to channel" + IRC_ENDLINE

// * Too many recipients
# define ERR_TOOMANYTARGETS(client, command) ":ft_irc 407 " + client + " " + command + " :Too many recipients" + IRC_ENDLINE

// * No recipient given
# define ERR_NORECIPIENT(client, command) ":ft_irc 411 " + client + " :No recipient given " + command + IRC_ENDLINE

// * No text to send
# define ERR_NOTEXTTOSEND(client) ":ft_irc 412 " + client + " :No text to send" + IRC_ENDLINE


// * COMMANDS

// WHOIS
// RPL_WHOISACTUALLY (338)
// RPL_WHOISMODES (379)
// RPL_WHOISSECURE (671)
// RPL_AWAY (301)
# define RPL_WHOISCERTFP(client, nick, fingerprint) ":ft_irc 276 " + client + " " + nick + " :has client certificate fingerprint " + fingerprint + IRC_ENDLINE
# define RPL_WHOISREGNICK(client, nick) ":ft_irc 307 " + client + " " + nick + " :has identified for this nick" + IRC_ENDLINE
# define RPL_WHOISUSER(client, nick, username, hostname, realname) ":ft_irc 311 " + client + " " + nick + " " + username + " " + hostname + " * :" + realname + IRC_ENDLINE
# define RPL_WHOISSERVER(client, nick, servername, serverinfo) ":ft_irc 312 " + client + " " + nick + " " + servername + " :" + serverinfo + IRC_ENDLINE
# define RPL_WHOISOPERATOR(client, nick) ":ft_irc 313 " + client + " " + nick + " :is an IRC operator" + IRC_ENDLINE
# define RPL_WHOISIDLE(client, nick, secs, signon) ":ft_irc 317 " + client + " " + nick + " " + idle + " " + signon + " :seconds idle, signon time" + IRC_ENDLINE
# define RPL_WHOISCHANNELS(client, nick, channels) ":ft_irc 319 " + client + " " + nick + " :" + channels + IRC_ENDLINE
# define RPL_WHOISSPECIAL(client, nick, especial) ":ft_irc 320 " + client + " " + nick + " :" + especial + IRC_ENDLINE
# define RPL_WHOISACCOUNT(client, nick, account) ":ft_irc 330 " + client + " " + nick + " " + account + " :is logged in as" + IRC_ENDLINE
# define RPL_WHOISACTUALLY(client, nick, user, host, server, ip, realname) ":ft_irc 338 " + client + " " + nick + " " + user + " " + host + " " + server + " " + ip + " :" + realname + IRC_ENDLINE
# define RPL_WHOISHOST(client, nick) ":ft_irc 378 " + client + " " + nick + " :is connecting from *@localhost 127.0.0.1" + IRC_ENDLINE
# define RPL_WHOISMODES(client, nick, modes) ":ft_irc 379 " + client + " " + nick + " :is using modes " + modes + IRC_ENDLINE
# define RPL_WHOISSECURE(client, nick) ":ft_irc 671 " + client + " " + nick + " :is using a secure connection" + IRC_ENDLINE
# define RPL_AWAY(client, nick, message) ":ft_irc 301 " + client + " " + nick + " :" + message + IRC_ENDLINE

// * LIST
# define RPL_LISTSTART(client) ":ft_irc 321 " + client + " :Channel :Users Name" + IRC_ENDLINE
# define RPL_LIST(client, channel, users, topic) ":ft_irc 322 " + client + " " + channel + " " + users + " :" + topic + IRC_ENDLINE
# define RPL_LISTEND(client) ":ft_irc 323 " + client + " :End of /LIST" + IRC_ENDLINE


// * INVITE
# define RPL_INVITING(client, nick, channel) ":ft_irc 341 " + client + " " + nick + " " + channel + IRC_ENDLINE

// * TOPIC
# define RPL_TOPIC(client, channel, topic) ":ft_irc 332 " + client + " " + channel + " :" + topic + IRC_ENDLINE
# define RPL_NOTICETOPIC(client, channel, topic) ":" + client + " TOPIC " + channel + " " + topic + IRC_ENDLINE

// * PRIVMSG
# define RPL_PRIVMSG(client, target, message) ":" + client + " PRIVMSG " + target + " :" + message + IRC_ENDLINE

// * MODE
# define RPL_MODE(channel, client, modes) ":" + client + " MODE " + channel + " " + modes + IRC_ENDLINE

// * JOIN
# define RPL_JOIN(client, channel) ":" + client + " JOIN " + channel + IRC_ENDLINE

// * PART
# define RPL_PART(client, channel, message) ":" + client + " PART " + channel + " " + message + IRC_ENDLINE

// * KICK
# define RPL_KICK(client, channel, nick, comment) ":" + client + " KICK " + channel + " " + nick + " " + comment + IRC_ENDLINE