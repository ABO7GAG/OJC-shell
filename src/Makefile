CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
LDFLAGS ?=
LDLIBS ?= -lreadline

SOURCES = main.c parser.c executor.c builtins.c config.c theme.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean test
all: ojcsh

ojcsh: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test:
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -I. -o test_parser tests/test_parser.c parser.c
	./test_parser
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_POSIX_C_SOURCE=200809L -I. -o test_config tests/test_config.c config.c theme.c
	./test_config
	rm -f test_parser test_config

clean:
	rm -f $(OBJECTS) ojcsh test_parser test_config
