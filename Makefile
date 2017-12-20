CC = gcc

CPPFLAGS =  \
-Iinclude/ \
-Iinclude/utils/ \
-D_GNU_SOURCE \
-MMD

CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic

LDLIBS = -lreadline -lhistory

VPATH = src/:src/utils/:src/commands/

FILES = \
my-dbg.c \
commands.c \
prompt.c \
array_utils.c \
string_utils.c \
cmd_quit.c \
cmd_info_regs.c \
binary.c

OBJS = $(FILES:%.c=%.o)

BIN = my-dbg

all: $(BIN)

$(BIN): $(OBJS)

debug: CFLAGS += -g3
debug: CFLAGS += -O0
debug: LDFLAGS += -g3
debug: clean all

clean:
	$(RM) $(BIN) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)
