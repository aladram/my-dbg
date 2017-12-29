#include "my-dbg.h"

#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>

#include "binary.h"
#include "commands.h"
#include "libunwind_wrapper.h"
#include "prompt.h"

int g_quit = 0;

void setup_modules(void)
{
    init_libunwind();
}

int main(int argc, char **argv)
{
    if (argc >= 2 && !strcmp(argv[1], "-h"))
    {
        printf("Usage: %s <path to binary> [arguments ...]\n", argv[0]);

        return 0;
    }

    if (argc >= 2)
    {
        setup_binary(argv + 1);

        setup_modules();
    }

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
