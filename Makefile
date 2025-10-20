NAME = webserv
CC = c++
FLAGS =  -Iincludes -Wall -Wextra -Werror -std=c++98 -MMD -MP
TEST_FLAGS = -Iincludes -std=c++98 -MMD -MP
RM = rm -rf

#============== SOURCES ==============#

SRCS_DIR = srcs
MAIN = main ProjectTools
WEBSERV = Webserv WebservRunner WebservManageClient WebservCleaning WebservHelpers

CONFIG = Config
CONFIG_GLOBAL = GlobalConfig GlobalConfigGetter GlobalConfigSetter
CONFIG_LOCATION = LocationConfig LocationConfigGetter LocationConfigSetter LocationConfigParsing
CONFIG_SERVER = ServerConfig ServerConfigGetter ServerConfigParsing
CONFIG_UTILS = Utils
CONFIG_TOKENIZER = AnalyzeToken Tokenizer TokenizerUtils

PARSING = Client ClientHelpers ClientReadWrite ClientGetSet

HTTPREQUEST = HttpRequest HttpRequestHelper HttpLocationConfig \
				HttpRequestError HttpRequestGetters \
				HttpRequestHandler HttpRequestHandlerHelpers \
				HttpRequestPostHandler HttpRequestDeleteHandler \
				HttpRequestSetters HttpRequestCGIHelpers \
				HttpRequestBodyParser HttpServerConfig

HTTPRESPONSE = HttpResponse HttpResponseHelper HttpResponseGetters \
				HttpResponseSetters HttpResponseCookie

CGI = CgiForkHelpers CgiHandler CgiHandlerFork CgiHandlerHelpers

SRCS = $(addsuffix .cpp, $(addprefix $(SRCS_DIR)/, $(MAIN))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/webserv/, $(WEBSERV))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/, $(CONFIG))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/global/, $(CONFIG_GLOBAL))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/location/, $(CONFIG_LOCATION))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/server/, $(CONFIG_SERVER))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/utils/, $(CONFIG_UTILS))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/config/tokenizer/, $(CONFIG_TOKENIZER))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/, $(PARSING))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/request/, $(HTTPREQUEST))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/parsing/response/, $(HTTPRESPONSE))) \
		$(addsuffix .cpp, $(addprefix $(SRCS_DIR)/cgi/, $(CGI))) \

H_WEBSERV = Includes Webserv
H_CONFIG = Config
H_CONFIG_GLOBAL = GlobalConfig
H_CONFIG_LOCATION = LocationConfig
H_CONFIG_SERVER = ServerConfig
H_CONFIG_UTILS = Utils
H_CONFIG_TOKENIZER = Token
H_PARSING = Client
H_REQUEST = HttpRequest
H_RESPONSE = HttpResponse
H_CGI = CgiHandler

HEADERS = $(addsuffix .hpp, $(addprefix $(SRCS_DIR)/webserv/, $(H_WEBSERV))) \
		$(SRCS_DIR)/config/Config.hpp \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/, $(H_CONFIG))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/global/, $(H_CONFIG_GLOBAL))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/location/, $(H_CONFIG_LOCATION))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/server/, $(H_CONFIG_SERVER))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/utils/, $(H_CONFIG_UTILS))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/config/tokenizer/, $(H_CONFIG_TOKENIZER))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/, $(H_PARSING))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/request/, $(H_REQUEST))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/parsing/response/, $(H_RESPONSE))) \
		$(addsuffix .hpp, $(addprefix $(SRCS_DIR)/cgi/, $(H_CGI))) \
		$(SRCS_DIR)/ProjectTools.hpp

#====== OBJECTS & DEPENDENCIES ======#

OBJDIR = objects
OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_SUBDIRS = \
	$(OBJDIR) \
	$(OBJDIR)/webserv \
	$(OBJDIR)/config/global \
	$(OBJDIR)/config/location \
	$(OBJDIR)/config/server \
	$(OBJDIR)/config/tokenizer \
	$(OBJDIR)/config/utils \
	$(OBJDIR)/parsing \
	$(OBJDIR)/parsing/request \
	$(OBJDIR)/parsing/response \
	$(OBJDIR)/cgi

DEPS = $(OBJS:.o=.d)

#=============== RULES ===============#

all:  $(NAME)

test: $(OBJS)
	@$(CC) $(TEST_FLAGS) $(OBJS) -o $(NAME)
	@echo "\n\033[1;32m ⚙️  WebServ compiled in production mode\033[0m ⚙️\n"

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "\n\033[1;32mWebServ compiled\033[0m 🔥🚀\n"

$(OBJDIR)/%.o: $(SRCS_DIR)/%.cpp Makefile | $(OBJ_SUBDIRS)
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
