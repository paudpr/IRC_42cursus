#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
	private:
		std::string				topic;
		std::string				name;
		t_channel_modes			mode;
		std::vector<Client*>	clients;
	public:
		Channel();
		~Channel();
		Channel(const Channel& copy);
		Channel& operator=(const Channel& other);
		Channel(std::string name);

		// Setter
		void	set_topic(const std::string topic);
		void	set_name(const std::string name);
		void	set_mode(t_channel_modes mode);

		//Getters
		std::string	get_topic(void) const;
		std::string	get_name(void) const;
		t_channel_modes	get_mode(void) const;
};
#endif