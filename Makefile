CPPFLAGS = -Iinclude/

CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic

VPATH = src/

FILES = my-dbg.c

OBJS = $(FILES:%.c=%.o)

BIN = my-dbg

all: $(BIN)

$(BIN): $(OBJS)

clean:
	$(RM) $(BIN) $(OBJS)
