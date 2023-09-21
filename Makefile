# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/13 07:21:01 by zmrabet           #+#    #+#              #
#    Updated: 2023/09/19 02:06:14 by zmrabet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = ircserv
CC		= c++
RM      = rm -rf
VERSION = -std=c++98
CFLAGS  = -Wall -Wextra -Werror  $(VERSION)
DEBUG   = -DDEBUG_MODE

V       = $(shell tput -Txterm setaf 5)
W       = $(shell tput -Txterm setaf 7)
Y       = $(shell tput -Txterm setaf 3)
G       = $(shell tput -Txterm setaf 2)
R       = $(shell tput -Txterm setaf 1)

INCLUDES   = Components/Components.hpp \
			 Components/Server/Server.hpp \
			 Components/Client/Client.hpp \
			 Utils/Utils.hpp \
			 Components/User/User.hpp \
			 Components/Channel/Channel.hpp

CLIENT     = Components/User/User.cpp \
			 Components/Channel/Channel.cpp \
			 Components/Client/Client.cpp



SERVER     = Components/Server/Server.cpp
UTILS      = Utils/Utils.cpp
MAIN	   = main.cpp
BIN        = obj/
SRC        = $(UTILS) $(CLIENT) $(SERVER) $(MAIN)
OBJ        = $(addprefix $(BIN),$(SRC:.cpp=.o))
BANNER     =    "\t\t$(V)██$(W)╗ $(V)██████$(W)╗   $(V)██████$(W)╗\n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)╔══$(V)██$(W)╗ $(V)██$(W)╔════╝\n"\
				"\t\t$(V)██$(W)║ $(V)██████$(W)╔╝ $(V)██$(W)║     \n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)╔══$(V)██$(W)╗ $(V)██$(W)║     \n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)║  $(V)██$(W)║ ╚$(V)██████$(W)╗\n"\
				"\t\t╚═╝ ╚═╝  ╚═╝  ╚═════╝\n"

all : $(NAME)

$(NAME) : $(OBJ)
	@echo "\n"
	@echo $(BANNER)
	@$(CC) $(CFLAGS)  $(OBJ) -o $@
	@echo "\t\t Generate Executable ( $(V)$@$(W) )\n"

$(BIN)%.o : %.cpp $(INCLUDES)
	@mkdir -p $(@D)
	@echo "$(V){$(W) Compiling ... $@ $(V)}$(W)"
	@$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

clean :
	@for obj in $(OBJ); do \
		echo "$(R)      - $$obj $(W)"; \
		$(RM) $$obj; \
	done
	@echo "Cleaning up object directory:"
	@echo "$(R)     - $(BIN)$(W)"
	@$(RM) $(BIN)
	

fclean : clean
	@echo "Cleaning up executable file:"
	@echo "$(R)     - $(NAME) $(W)"
	@$(RM) $(NAME)

re : fclean all

.PHONY : all clean fclean re