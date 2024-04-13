CC            = gcc
CFLAGS        = -pedantic -W -Wall -Wextra \
                -Wconversion -Wswitch-enum \
                -Werror -std=c99 -O0 -g

EXEC          = example

all: $(EXEC)

$(EXEC): $(EXEC).c version.h Makefile
	$(CC) $(CFLAGS) $(filter %.c %.o %.s,$^) -o $@

clean:
	rm -rf $(EXEC) *.o *.d

.PHONY: all clean
