NAME		=	ircbot

CXX			=	c++
CXXFLAGS	=	-std=c++98 -Wall -Werror -Wextra -g3
CXXFLAGS	+=	-I $(INC_DIR)
DEBUGFLAGS = -fsanitize=address -g3

OBJ_DIR = objs
SRC_DIR = srcs
INC_DIR = incl

SRCS = main.cpp \
		utils.cpp \
		Bot.cpp \
		botUtils.cpp \
		messageHandler.cpp \
		connectionHandler.cpp \

OBJS = $(SRCS:%.cpp=%.o)
OBJS_DIR = $(addprefix $(OBJ_DIR)/,$(OBJS))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR): | $(OBJ_DIR)

$(NAME): $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS_DIR) -o $(NAME)

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR) 2> /dev/null

re: fclean all

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

.PHONY: re clean fclean debug