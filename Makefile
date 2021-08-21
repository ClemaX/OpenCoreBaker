NAME = ocbaker
DIST = debug

RUN = ./$(BINDIR)/$(NAME) sample-recipe.plist dest/test

OS := $(shell uname)
ARCH = x86_64

CC = clang
LD = clang

SRCDIR = src
INCDIR = include

OBJDIR = objs/$(DIST)
BINDIR = bin/$(DIST)
DEPDIR = .deps/$(DIST)

CFLAGS = -Wall -Wextra -Werror -DOCB_DIST=$(DIST) -I$(INCDIR)
DFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
LFLAGS = -lplist-2.0.3 -lcurl -lzip

#ifeq ($(OS), Darwin)
#	LFLAGS += -lSystem -macosx_version_min 10.13
#endif

ifeq ($(DIST), debug)
	CFLAGS += -g3 -fsanitize=address -fsanitize=undefined# -fsanitize=leak
	LFLAGS += -g3 -fsanitize=address -fsanitize=undefined# -fsanitize=leak
endif

SRCS = $(addprefix $(SRCDIR)/,\
	main.c\
	recipe.c\
	vitamin.c\
	url.c\
	url_queue.c\
	url_dl.c\
	archive.c\
	file_utils.c\
	config.c\
)

DEPS = $(SRCS:$(SRCDIR)/%.c=$(DEPDIR)/%.d)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

COMPILE.c = $(CC) $(DFLAGS) $(CFLAGS) -c
COMPILE.o = $(LD) $(LFLAGS)

all: $(BINDIR)/$(NAME)

$(DEPDIR) $(OBJDIR) $(BINDIR):
	@echo "MK $@"
	mkdir -p $@

#$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d | $(DEPDIR) $(OBJDIR)
	@echo "CC $<"
	$(COMPILE.c) $< -o $@

$(DEPS): $(DEPDIR)/%.d:
include $(wildcard $(DEPS))

$(BINDIR)/$(NAME): $(OBJS) | $(BINDIR)
	@echo "LD $@"
	$(COMPILE.o) $^ -o $@

clean:
	@echo "RM $(OBJDIR)"
	rm -rf $(OBJDIR)
	@echo "RM $(DEPDIR)"
	rm -rf $(DEPDIR)

fclean: clean
	@echo "RM $(BINDIR)"
	rm -rf $(BINDIR)

re: fclean all

run: $(BINDIR)/$(NAME)
	@echo "RUN $(RUN)"
	@echo
	$(RUN)

.PHONY = run

$(VERBOSE).SILENT:
