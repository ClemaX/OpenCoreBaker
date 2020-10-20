NAME = ocbaker
DIST = debug

RUN = ./$(BINDIR)/$(NAME) sample-recipe.plist dest/test

ARCH = x86_64

CC = clang
LD = ld

SRCDIR = srcs
INCDIR = includes

OBJDIR = objs
BINDIR = bin
DEPDIR = .deps

CFLAGS = -Wall -Wextra -g3 -DOCB_DIST=$(DIST) -I$(INCDIR)
DFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
LFLAGS = -arch $(ARCH) -macosx_version_min 10.13 -lSystem -lplist-2.0.3 -lcurl

SRCS = $(addprefix srcs/, main.c recipe.c vitamin.c url.c config.c)

DEPS = $(SRCS:$(SRCDIR)/%.c=$(DEPDIR)/%.d)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

COMPILE.c = $(CC) $(DFLAGS) $(CFLAGS) -arch $(ARCH) -c

all: $(BINDIR)/$(NAME)


$(DEPDIR):
	@echo "MK $@"
	@mkdir -p $@

$(OBJDIR):
	@echo "MK $@"
	@mkdir -p $@

$(BINDIR):
	@echo "MK $@"
	@mkdir -p $@

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d | $(DEPDIR) $(OBJDIR)
	@echo "CC $<"
	@$(COMPILE.c) $< -o $@

$(DEPS): $(DEPDIR)/%.d:
include $(wildcard $(DEPS))

$(BINDIR)/$(NAME): $(OBJS) $(BINDIR)
	@echo "LD $@"
	@$(LD) $(LFLAGS) $(OBJS) -o $@

clean:
	@echo "RM $(OBJDIR)"
	@rm -rf $(OBJDIR)
	@echo "RM $(DEPDIR)"
	@rm -rf $(DEPDIR)

fclean: clean
	@echo "RM $(BINDIR)"
	@rm -rf $(BINDIR)

re: fclean all

run: $(BINDIR)/$(NAME)
	@echo "RUN $(RUN)"
	@echo
	@$(RUN)


.PHONY = run
