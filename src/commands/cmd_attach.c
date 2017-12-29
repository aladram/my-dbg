#include <err.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "binary.h"
#include "commands.h"
#include "exceptions.h"
#include "format_utils.h"
#include "my_syscalls.h"
#include "my-dbg.h"

static void attach_process(pid_t pid)
{
    g_pid = pid;

    enum my_exception my_ex = None;

    try
    {
        my_ptrace(PTRACE_ATTACH, NULL, NULL);

        int wstatus;

        my_wait(&wstatus);

        if (!WIFSTOPPED(wstatus) || WSTOPSIG(wstatus) != SIGSTOP)
        {
            warnx("An error occured while trying to debug program");

            throw(WaitException);
        }
    }
    catch (PtraceException)
    {
        my_ex = ex;
    }
    catch (WaitException)
    {
        my_ex = ex;
    }
    etry;

    if (my_ex != None)
    {
        g_pid = 0;

        throw(PtraceException);
    }

    setup_modules();
}

static void cmd_attach(size_t argc, char **argv)
{
    if (g_pid)
    {
        warnx("A process is already attached");

        return;
    }

    if (argc < 2)
    {
        warnx("No process ID specified");

        return;
    }

    size_t pid = read_size(argv[1]);

    if (pid >= (size_t) (1 << (sizeof(pid_t) * 8 - 1)))
    {
        warnx("Invalid process ID specified");

        return;
    }

    attach_process((pid_t) pid);
}

register_command(attach,
                 NO_PROGRAM_REQUIRED,
                 cmd_attach,
                 "Attach process identified by the given process ID",
                 "attach <pid>");
