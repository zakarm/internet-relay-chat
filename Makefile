NAME    = ircserv
CC		= c++
RM      = rm -rf
VERSION = -std=c++98
CFLAGS  = -Wall -Wextra -Werror  $(VERSION) -g  #-fsanitize=address
DEBUG   = -DDEBUG_MODE

V       = $(shell tput -Txterm setaf 5)
W       = $(shell tput -Txterm setaf 7)
Y       = $(shell tput -Txterm setaf 3)
G       = $(shell tput -Txterm setaf 2)
R       = $(shell tput -Txterm setaf 1)

INCLUDES   = Components/Components.hpp \
			 Components/Server/Server.hpp \
			 Utils/Utils.hpp \
			 Components/User/User.hpp \
			 Components/Channel/Channel.hpp

CLIENT     = Components/User/User.cpp \
			 Components/Channel/Channel.cpp \



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
	./ircserv 6667 pass	

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