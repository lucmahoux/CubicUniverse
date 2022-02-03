CC = gcc
IMPOSED_FLAGS = -Wall -Wextra -std=c99
EXTRA_FLAGS = -Werror -pedantic -I..
GL_INCLUDES = $(shell pkg-config --cflags glfw3)
GL_LIBS = $(shell pkg-config --static --libs glfw3 glew opengl)

mode = debug
ifeq ($(mode),debug)
EXTRA_FLAGS += -g
endif

sanitize = no
ifeq ($(sanitize),yes)
EXTRA_FLAGS += -fsanitize=address
endif

LIBUTILS_DIR = ../utils
LIBUTILS_SRC = utils.c matrix.c vector.c list.c
LIBUTILS_SRC := $(addprefix $(LIBUTILS_DIR)/,$(LIBUTILS_SRC))

LIBGRAPHICS_DIR = graphics
LIBGRAPHICS_SRC = shader.c open_gl.c cube.c

LIBSTRUCT_DIR = ../struct
LIBSTRUCT_SRC = block.c
LIBSTRUCT_SRC := $(addprefix $(LIBSTRUCT_DIR)/,$(LIBSTRUCT_SRC))

ALL_FLAGS = $(IMPOSED_FLAGS) $(EXTRA_FLAGS) $(CPP_FLAGS)

export

EXECS = graphics
MAKE += --no-print-directory
SUB_RULES = all clean

.PHONY = all $(EXECS)

all: $(EXECS)

define explicit_template =
$(1): phony_explicit
	@$(MAKE) -C $(1)
endef

define explicit_sub_template =
$(1)_$(2): phony_explicit
	@$$(MAKE) -C $(1) $(2)
endef

$(foreach EXEC,$(EXECS),$(eval $(call explicit_template,$(EXEC))))

$(foreach EXEC,$(EXECS),$(foreach RULE,$(SUB_RULES),\
	$(eval $(call explicit_sub_template,$(EXEC),$(RULE)))))

clean: $(addsuffix _clean,$(EXECS))

%_run: % phony_explicit
	@echo "Running $<..."
	@echo "========================="
	@-cd $< && ./$<
	@echo "========================="
	@echo "Finished running $<"
	@echo ""

run: $(addsuffix _run,$(EXECS))

phony_explicit:
