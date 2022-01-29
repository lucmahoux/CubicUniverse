INCLUDES = $(GL_INCLUDES)
LIBS = $(GL_LIBS)
SRC = main.c $(LIBGRAPHICS_SRC) $(LIBUTILS_SRC)
CFLAGS = $(ALL_FLAGS) $(INCLUDES) $(LIBS)
EXEC = graphics

.PHONY = phony_explicit all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(EXEC)

phony_explicit:
