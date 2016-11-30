# Declaration of variables
CC = gcc
<<<<<<< HEAD
CC_FLAGS = -std=gnu99 -Wall
LD_FLAGS = -lm -lpthread
=======
CC_FLAGS = -std=gnu99 
LD_FLAGS = -lm -pthread
>>>>>>> 33f39cab253cef767f61c85dd59a6945cd48ab4d

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

