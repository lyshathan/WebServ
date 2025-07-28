NAME = WebServ
CC = c++
FLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98 -MMD -MP -g3
TEST_FLAGS = -Iincludes -std=c++98 -MMD -MP -g3
RM = rm -rf

#============== SOURCES ==============#

SRCS_DIR = srcs
MAIN = main
SERVER = Server ServerRunner ServerManageClient ServerErrors
PARSING = Client HttpRequest HttpResponse

SRCS = $(addsuffix .cpp, $(addprefix $(SRCS_DIR)/, $(MAIN))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/server/, $(SERVER))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/, $(PARSING))) \

# HEADERS = includes/Server.hpp includes/Includes.hpp

#====== OBJECTS & DEPENDENCIES ======#

OBJDIR = objects
OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_SUBDIRS = \
	$(OBJDIR) \
	$(OBJDIR)/server \
	$(OBJDIR)/parsing

DEPS = $($(OBJDIR)/%.o=.d)

#=============== RULES ===============#

all:  $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\n\033[1;32mWebServ compiled\033[0m 🔥🚀\n"

$(OBJDIR)/%.o: $(SRCS_DIR)/%.cpp Makefile | $(OBJ_SUBDIRS) #$(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

$(OBJ_SUBDIRS):
	@mkdir -p $@

#============== CLEANING ==============#
clean:
	$(RM) $(OBJDIR) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
