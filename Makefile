NAME = WebServ
CC = c++
FLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98 -MMD -MP -g3
TEST_FLAGS = -Iincludes -std=c++98 -MMD -MP -g3
RM = rm -rf

#============== SOURCES ==============#

SRCS_DIR = sources
SRCS_FILES = main Server ServerRunner

SRCS = $(addsuffix .cpp, $(addprefix $(SRCS_DIR)/, $(SRCS_FILES)))


#============== HEADERS ==============#

INCLUDES = includes

#====== OBJECTS & DEPENDENCIES ======#

OBJDIR = objects

OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJDIR)/%.o)
TEST_OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJDIR)/test_%.o)
DEPS = $(OBJS:.o=.d)

#=============== RULES ===============#

all : $(NAME)

test : $(TEST_OBJS) Makefile
	$(CC) $(TEST_FLAGS) $(TEST_OBJS) -o $(NAME)

$(OBJDIR)/test_%.o : $(SRCS_DIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(TEST_FLAGS) -c $< -o $@
	
$(OBJDIR)/%.o : $(SRCS_DIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c $< -o $@

$(NAME) : $(OBJS) Makefile
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

#============== CLEANING ==============#
clean:
	$(RM) $(OBJDIR) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
