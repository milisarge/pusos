#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"

extern mutex_t mu_lock_raft1;

extern mu_t scope_raft1;

extern int raft_sender(char *cmd, int term);
