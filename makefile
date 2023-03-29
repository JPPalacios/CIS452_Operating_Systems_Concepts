CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -pthread
RM = rm -f
QUIET = @
OBJDIR = obj
SRCDIR = src

.PHONY: all run clean

all: $(OBJDIR)/main

$(OBJDIR)/main: $(OBJDIR)/main.o | $(OBJDIR)
	$(QUIET)$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/main.h | $(OBJDIR)
	$(QUIET)$(CC) $(CFLAGS) -c $< -o $@

run: clean $(OBJDIR)/main
	$(QUIET)./$(OBJDIR)/main

clean:
	$(QUIET)$(RM) $(OBJDIR)/*.o $(OBJDIR)/main

$(OBJDIR):
	$(QUIET)mkdir $(OBJDIR)
