NAME = Webserv
CC = c++
FLAGS =  -Iincludes -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3
TEST_FLAGS = -Iincludes -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3
RM = rm -rf

#============== SOURCES ==============#

SRCS_DIR = srcs
MAIN = main ProjectTools
WEBSERV = Webserv WebservRunner WebservManageClient WebservCleaning WebservConversion
GENERAL = General
CONFIG_PARSER = Config \
				GlobalConfig GlobalConfig_Get_Set\
				ServerConfig ServerConfigParsing ServerConfig_Get_Set\
				LocationConfig LocationConfigParsing LocationConfig_Get_Set\
				Utils

CONFIG_TOKENIZER = AnalyzeToken Tokenizer TokenizerUtils

PARSING = Client

HTTPREQUEST = HttpRequest HttpRequestHelper HttpLocationConfig \
				HttpServerConfig HttpRequestNotFound

HTTPRESPONSE = HttpResponse HttpResponseHelper HttpResponseGetters \
				HttpResponseSetters

SRCS = $(addsuffix .cpp, $(addprefix $(SRCS_DIR)/, $(MAIN))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/webserv/, $(WEBSERV))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/, $(GENERAL))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/parser/, $(CONFIG_PARSER))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/tokenizer/, $(CONFIG_TOKENIZER))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/, $(PARSING))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/request/, $(HTTPREQUEST))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/response/, $(HTTPRESPONSE)))

H_WEBSERV = Includes Webserv
H_CONFIG_PARSER = Config GlobalConfig LocationConfig ServerConfig Utils
H_CONFIG_TOKENIZER = Token
H_PARSING = Client
H_REQUEST = HttpRequest
H_RESPONSE = HttpResponse

HEADERS = $(addsuffix .hpp, $(addprefix $(SRCS_DIR)/webserv/, $(H_WEBSERV))) \
		$(SRCS_DIR)/config/General.hpp \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/parser/, $(H_CONFIG_PARSER))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/tokenizer/, $(H_CONFIG_TOKENIZER))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/, $(H_PARSING))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/request/, $(H_REQUEST))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/response/, $(H_RESPONSE))) \
		$(SRCS_DIR)/ProjectTools.hpp

#====== OBJECTS & DEPENDENCIES ======#

OBJDIR = objects
OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_SUBDIRS = \
	$(OBJDIR) \
	$(OBJDIR)/webserv \
	$(OBJDIR)/config/parser \
	$(OBJDIR)/config/tokenizer \
	$(OBJDIR)/parsing \
	$(OBJDIR)/parsing/request \
	$(OBJDIR)/parsing/response

DEPS = $($(OBJDIR)/%.o=.d)

#=============== RULES ===============#

all:  $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\n\033[1;32mWebServ compiled\033[0m 🔥🚀\n"

$(OBJDIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADERS) Makefile | $(OBJ_SUBDIRS)
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
