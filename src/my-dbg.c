#include "my-dbg.h"

#include <err.h>
#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>

#include "binary.h"
#include "commands.h"
#include "libunwind_wrapper.h"
#include "prompt.h"

int g_quit = 0;

int main(int argc, char **argv)
{
    if (argc <= 1)
        errx(1, "Usage: %s <path to binary> [arguments ...]", argv[0]);

    setup_binary(argv + 1);

    init_libunwind();

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

        for (size_t i = 0; tokens && tokens[i]; ++i)
            free(tokens[i]);

        free(tokens);
    }

    destroy_libunwind();

    return 0;
}
