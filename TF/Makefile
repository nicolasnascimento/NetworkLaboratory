# Declaration of variables
CC = clang
CC_FLAGS = -std=gnu99 -Wall
LD_FLAGS = -lm

# File names
EXEC = run
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

# Main Target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LD_FLAGS)

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean: 
	rm -f $(EXEC) $(OBJECTS)

