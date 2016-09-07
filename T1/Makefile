# Declaration of variables
CC = gcc
CC_FLAGS = -w -std=gnu99 -Wall

# File names
EXEC = run
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

# Main Target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean: 
	rm -f $(EXEC) $(OBJECTS)

