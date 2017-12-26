#include "prompt.h"

#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>

char *prompt(void)
{
    return readline(isatty(STDIN_FILENO) ? MY_PROMPT : NULL);
}
