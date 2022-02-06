INCLUDES = $(GL_INCLUDES)
LIBS = $(GL_LIBS)
SRC = main.c $(LIBRENDER_SRC) $(LIBUTILS_SRC) $(LIBINPUT_SRC) $(LIBGAME_SRC)
CFLAGS = $(ALL_FLAGS) $(INCLUDES) $(LIBS)
EXEC = game

.PHONY = phony_explicit all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(EXEC)

phony_explicit:
