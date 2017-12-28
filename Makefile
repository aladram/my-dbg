CC = gcc

CPPFLAGS =  \
-Iinclude/ \
-Iinclude/utils/ \
-Iinclude/wrappers/ \
-Icapstone/ \
-D_GNU_SOURCE \
-MMD

CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic

LDLIBS = \
-ldl \
-lreadline -lhistory \
-lcapstone \
-lunwind -lunwind-ptrace -lunwind-x86_64 

VPATH = src/:src/utils/:src/commands/:src/wrappers/

FILES = \
my-dbg.c \
commands.c \
prompt.c \
array_utils.c \
string_utils.c \
memory_utils.c \
format_utils.c \
exceptions.c \
cmd_quit.c \
cmd_info_regs.c \
cmd_info_memory.c \
cmd_break.c \
cmd_continue.c \
cmd_step_instr.c \
cmd_examine.c \
cmd_backtrace.c \
cmd_tbreak.c \
cmd_next_instr.c \
binary.c \
breakpoints.c \
registers.c \
memory.c \
mem_mappings.c \
errors.c \
libunwind_wrapper.c \
capstone_wrapper.c

OBJS = $(FILES:%.c=%.o)

BIN = my-dbg

all: $(BIN)

$(BIN): $(OBJS)

debug: CFLAGS += -g3
debug: CFLAGS += -O0
debug: LDFLAGS += -g3
debug: clean all

clean-objs:
	$(RM) $(OBJS) $(OBJS:.o=.d)

clean:
	$(RM) $(BIN) $(OBJS) $(OBJS:.o=.d)

-include $(OBJS:.o=.d)
