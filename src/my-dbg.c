#include "my-dbg.h"

#include <err.h>
#include <readline/history.h>
#include <stdlib.h>

#include "binary.h"
#include "commands.h"
#include "prompt.h"

int g_quit = 0;

int main(int argc, char **argv)
{
    if (argc <= 1)
        errx(1, "Usage: %s <path to binary> [arguments ...]", argv[0]);

    setup_binary(argv + 1);

    history_word_delimiters = " \t";

    while (!g_quit)
    {
        char *str = prompt();

        if (!str)
            break;

        char **tokens = history_tokenize(str);

        if (tokens && *tokens)
            add_history(str);

        free(str);

        if (tokens && *tokens)
            run_command(*tokens, tokens);

        free(tokens);
    }

    return 0;
}
