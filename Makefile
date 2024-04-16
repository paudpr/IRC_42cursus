NAME	= ircserv
BOT		= ircbot

CXX = c++
CXXFLAGS = -std=c++98 -Wall -Werror -Wextra -g3
CXXFLAGS += -I $(INC_DIR)
LEAKSFLAG = -DLEAKS=1
FDFLAG = -DFD=1

OBJ_DIR = objs
SRC_DIR = srcs
INC_DIR = incl

SRCS = main.cpp \
		Server.cpp \
		Client.cpp \
		Message.cpp \
		Channel.cpp \
		commands.cpp \
		utils.cpp 

OBJS = $(SRCS:%.cpp=%.o)
OBJS_DIR = $(addprefix $(OBJ_DIR)/,$(OBJS))

all: $(NAME)

leaks: CXXFLAGS += $(LEAKSFLAG)
leaks: all

fd: CXXFLAGS += $(FDFLAG)
fd: all

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR): | $(OBJ_DIR)

$(NAME): $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS_DIR) -o $(NAME)

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR) 2> /dev/null

re: fclean all

debug: CXXFLAGS += -fsanitize=address -g3
debug: $(NAME)

#######################
#		  Bot         #
#######################
bot: $(BOT)
	cp bot/ircbot .
	cp -rf bot/conf/API.conf ./conf/API.conf

$(BOT):
	@make -C bot

bot_clean:
	@make clean -C bot

bot_fclean:
	@make fclean -C bot
	@if [ -f "ircbot" ]; then rm ircbot; fi
	@if [ -f "./conf/API.conf" ]; then rm ./conf/API.conf; fi
bot_re:
	@make re -C bot
	@if [ -f "ircbot" ]; then rm ircbot; fi
	@if [ -f "./conf/API.conf" ]; then rm ./conf/API.conf; fi
bot_leaks:
	@make leaks -C bot
bot_fd:
	@make fd -C bot

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

all_clean: fclean bot_fclean

.PHONY: re clean fclean debug all bot bot_clean bot_fclean bot_re bot_leaks bot_fd
