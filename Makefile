NAME    = ircserv
NAME_BONUS = bot
CC		= c++
RM      = rm -rf
VERSION = -std=c++98
CFLAGS  = -Wall -Wextra -Werror  $(VERSION)   -fsanitize=address
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


SERVER     = Components/Server/Server.cpp \
			 Components/Server/Commands.cpp \
			 Components/Server/Connection.cpp

UTILS      = Utils/Utils.cpp


MAIN	   = main.cpp

BOT        = bot.cpp
MAIN_BONUS = bot/main.cpp
BIN        = obj/
BIN_BONUS  = obj_bounus/
SRC        = $(UTILS) $(CLIENT) $(SERVER) $(MAIN)
SRC_BONUS  = $(BOT) $(MAIN_BONUS)

OBJ        = $(addprefix $(BIN),$(SRC:.cpp=.o))
OBJ_BONUS  = $(addprefix $(BIN_BONUS),$(SRC_BONUS:.cpp=.o))

BANNER     =    "\t\t$(V)██$(W)╗ $(V)██████$(W)╗   $(V)██████$(W)╗\n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)╔══$(V)██$(W)╗ $(V)██$(W)╔════╝\n"\
				"\t\t$(V)██$(W)║ $(V)██████$(W)╔╝ $(V)██$(W)║     \n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)╔══$(V)██$(W)╗ $(V)██$(W)║     \n"\
				"\t\t$(V)██$(W)║ $(V)██$(W)║  $(V)██$(W)║ ╚$(V)██████$(W)╗\n"\
				"\t\t╚═╝ ╚═╝  ╚═╝  ╚═════╝\n"

all : $(NAME)

bonus : $(NAME_BONUS)

$(NAME) : $(OBJ)
	@echo "\n"
	@echo $(BANNER)
	@$(CC) $(CFLAGS)  $(OBJ) -o $@
	@echo "\t\t Generate Executable ( $(V)$@$(W) )\n"

$(NAME_BONUS) : $(OBJ_BONUS)
	@echo "\n"
	@echo $(BANNER)
	@$(CC) $(CFLAGS)  $(OBJ_BONUS) -o $@
	@echo "\t\t Generate Executable ( $(V)$@$(W) )\n"


$(BIN)%.o : %.cpp $(INCLUDES)
	@mkdir -p $(@D)
	@echo "$(V){$(W) Compiling ... $@ $(V)}$(W)"
	@$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

$(BIN_BONUS)%.o : %.cpp $(INCLUDES)
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

	@for obj in $(OBJ_BONUS); do \
		echo "$(R)      - $$obj $(W)"; \
		$(RM) $$obj; \
	done
	@echo "Cleaning up object directory:"
	@echo "$(R)     - $(BIN_BONUS)$(W)"
	@$(RM) $(BIN_BONUS)
	

fclean : clean
	@echo "Cleaning up executable file:"
	@echo "$(R)     - $(NAME) $(W)"
	@$(RM) $(NAME)
	@echo "$(R)     - $(NAME_BONUS) $(W)"
	@$(RM) $(NAME_BONUS)

re : fclean all

.PHONY : all clean fclean re