INCLUDES = $(GL_INCLUDES) $(CRITERION_INCLUDES)
LIBS = $(GL_LIBS) $(CRITERION_LIBS)
SRC = main.c $(LIBUTILS_SRC) $(LIBTESTS_SRC)
CFLAGS = $(ALL_FLAGS) $(INCLUDES) $(LIBS)
EXEC = tests

.PHONY = phony_explicit all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(EXEC)

phony_explicit:
