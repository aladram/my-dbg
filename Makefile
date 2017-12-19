CC = gcc

CPPFLAGS =  \
-Iinclude/ \
-Iinclude/utils/ \
-D_GNU_SOURCE \
-MMD

CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic

LDLIBS = -lreadline -lhistory

VPATH = src/:src/utils/

FILES = \
my-dbg.c \
commands.c \
prompt.c \
array_utils.c \
string_utils.c

OBJS = $(FILES:%.c=%.o)

BIN = my-dbg

all: $(BIN)

$(BIN): $(OBJS)

clean:
	$(RM) $(BIN) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)
