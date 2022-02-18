CC = gcc
IMPOSED_FLAGS = -Wall -Wextra -std=c99
EXTRA_FLAGS = -Werror -pedantic -I..

GL_INCLUDES = $(shell pkg-config --cflags glfw3)
GL_LIBS = $(shell pkg-config --static --libs glfw3 glew opengl)

CRITERION_INCLUDES = $(shell pkg-config --cflags criterion)
CRITERION_LIBS = $(shell pkg-config --static --libs criterion)

mode = debug
ifeq ($(mode),debug)
EXTRA_FLAGS += -g
endif

sanitize = no
ifeq ($(sanitize),yes)
EXTRA_FLAGS += -fsanitize=address
endif

LIBUTILS_DIR = ../utils
LIBUTILS_SRC = utils.c matrix.c vector.c list.c hashmap.c \
			   shader.c open_gl.c window.c
LIBUTILS_SRC := $(addprefix $(LIBUTILS_DIR)/,$(LIBUTILS_SRC))

LIBRENDER_DIR = ../render
LIBRENDER_SRC = block.c camera.c blockstates.c block_list_loader.c chunk.c \
				chunk_loader.c block_utils.c
LIBRENDER_SRC := $(addprefix $(LIBRENDER_DIR)/,$(LIBRENDER_SRC))

LIBINPUT_DIR = ../input
LIBINPUT_SRC = 
LIBINPUT_SRC := $(addprefix $(LIBINPUT_DIR)/,$(LIBINPUT_SRC))

LIBGAME_DIR = ../game
LIBGAME_SRC = game.c

LIBTESTS_DIR = ../tests
LIBTESTS_SRC =

ALL_FLAGS = $(IMPOSED_FLAGS) $(EXTRA_FLAGS) $(CPP_FLAGS)

export

EXECS = game tests
MAKE += --no-print-directory
SUB_RULES = all clean

.PHONY = all $(EXECS)

all: clean $(EXECS)

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

run: clean $(addsuffix _run,$(EXECS))

phony_explicit:
