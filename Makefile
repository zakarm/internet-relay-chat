# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/13 07:21:01 by zmrabet           #+#    #+#              #
#    Updated: 2023/09/14 01:41:35 by zmrabet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = ircserv
CC		= c++
RM      = rm -rf
VERSION = -std=c++98
CFLAGS  = -Wall -Wextra -Werror $(VERSION)

V       = $(shell tput -Txterm setaf 5)
W       = $(shell tput -Txterm setaf 7)
Y       = $(shell tput -Txterm setaf 3)
G       = $(shell tput -Txterm setaf 2)
R       = $(shell tput -Txterm setaf 1)

INCLUDES   = Components/Components.hpp \
			 Components/Server/Server.hpp

SERVER     = Components/Server/Server.cpp
MAIN	   = main.cpp
BIN        = obj/
SRC        = $(SERVER) $(MAIN)
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
	@$(CC) $(CFLAGS) $(OBJ) -o $@
	@echo "\t\t Generate Executable ( $(V)$@$(W) )\n"

$(BIN)%.o : %.cpp $(INCLUDES)
	@mkdir -p $(@D)
	@echo "$(V){$(W) Compiling ... $@ $(V)}$(W)"
	@$(CC) $(CFLAGS) -c $< -o $@

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