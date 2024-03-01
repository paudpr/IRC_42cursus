#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
	private:
		std::string		topic;
		t_channel_modes	mode;
	public:
		Channel();
		~Channel();
		Channel(const Channel& copy);
		Channel& operator=(const Channel& other);
		Channel(std::string topic);
}

#endif