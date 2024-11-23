# Made by Serhij Čepil
# FIT VUT Student
# https://github.com/sipxi
# 09/10/2024

# The code is based on VUT FIT C Practicals


# Compiler and flags
CC 			= gcc #! CHANGE BASED ON YOUR COMPLIER
CFLAGS 		= -std=c11 -Wextra

# Executable names
EXEC        = figsearch
# Source files
MAIN_SRC    = src/figsearch.c

# Targets
.PHONY: all run clean test

# Default target to build both the main and test executables
all: $(EXEC)
	
# Build the main executable
$(EXEC): $(MAIN_SRC) 
	$(CC) $(CFLAGS) -o $@ $^

# Run the main program
run: $(EXEC)
	./$(EXEC)

test: $(EXEC)
	./$(EXEC) hline obrazek.txt


# Clean up compiled files
clean:
	@echo "Cleaning up all executables..."
	@if exist *.exe del *.exe
	@if exist *.o del *.o

# End of Makefile