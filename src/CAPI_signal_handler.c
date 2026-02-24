#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "CAPI_internal.h"

static volatile sig_atomic_t keep_running = 1;

static void sigchld_handler(int s)
{
    (void) s;
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

static void sigint_handler(int s)
{
    (void) s;
    keep_running = 0;
}

static struct sigaction sa_sigint = {.sa_handler = sigint_handler, .sa_flags = SA_RESTART};
static struct sigaction sa_sigchild = {.sa_handler = sigchld_handler, .sa_flags = SA_RESTART};

CAPI_ErrorCode CAPI_RegisterSignalActions()
{
    if (sigaction(SIGCHLD, &sa_sigchild, NULL) == -1 || sigaction(SIGINT, &sa_sigint, NULL) == -1)
    {
        CAPI_SetErrorCode(CAPI_ERR_SINGAL_REG, "Unable to register signal actions: %s", strerror(errno));
        return CAPI_ERR_SINGAL_REG;
    }

    return CAPI_SUCCESS;
}

int CAPI_ShouldKeepRunning()
{
    return keep_running;
}
