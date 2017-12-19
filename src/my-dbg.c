#include "my-dbg.h"

#include <readline/history.h>
#include <stdlib.h>

#include "commands.h"
#include "prompt.h"

int main(int argc, char **argv)
{
    (void)argc;

    (void)argv;

    while (1)
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
    }

    return 0;
}
