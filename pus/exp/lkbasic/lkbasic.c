#include "ubasic.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>


# include "program.h"
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static int lkbasic(int argc, const console_cmd_args *argv) {
  ubasic_init(program);

  do {
    ubasic_run();
  } while(!ubasic_finished());
  return 0;
}


STATIC_COMMAND_START
STATIC_COMMAND("lkbasic", "lkbasic tests", &lkbasic)
STATIC_COMMAND_END(lkbasic);

APP_START(lkbasic)
APP_END
