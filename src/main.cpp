

#include "common.h"

#include "KBScreen.hpp"
#include "KBDialog.hpp"


void validate_args(int argc, char * const argv[]);
void exec_command(int argc, char * const argv[]);

const char * Mesg  = nullptr;
bool         DEBUG = false;

int main(int argc, char * const argv[])
{
    validate_args(argc, argv);

    KBScreen screen;

    // Connect to Unix domain socket for events
    auto srvr = NXUnixPacketSocket::CreateClient("/tmp/kb-gpio");

    if (!srvr.valid())
    {
        fprintf(stderr, "Cannot connect to kb-gpio process\n");
        sleep(1);
        exit(1);
    }

    srvr.send_msg("kb-req");        D("Sent kb-req");
    srvr.recv_ack();                D("Received ack");

    while (true)
    {
        KBDialog dialog(&screen, &srvr);

        int choice = dialog.confirm(Mesg);

        if (false)
            fprintf(stderr, "Chose: %d\n", choice);

        if (choice == 1)
        {
            exec_command(argc - 3, argv + 3);
        }
        else
            exit(2);
    }

}

void exec_command(int argc, char * const argv[])
{
    char * const newenviron[] = { NULL };
    execve(argv[0], argv, newenviron);
    perror("execve");
    exit(EXIT_FAILURE);
}

void arg_help()
{
    fprintf(stderr, "usage: kb-req 'message' -- <command> [<args>]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "   ex: kb-req 'Allow ssh login?' -- /bin/bash -l\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "       Set DEBUG env var for debug messages.\n");
    exit(1);
}

void validate_args(int argc, char * const argv[])
{
    // cmd mesg -- next in chain
    if (argc < 4)
        arg_help();

    if (NXCStr{"--"} != argv[2])
        arg_help();

    Mesg = argv[1];

    if (getenv("DEBUG"))
        DEBUG = true;
}
