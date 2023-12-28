#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>
#define LINE_LEN 64

static int pos = 0;

#include "mu.h"
#include "num.h"
#include "str.h"
#include "tbl.h"
#include "fn.h"
#include "sys.h"
#include "vm.h"

// Mu state

#include <lib/setjmp.h>
jmp_buf mu_error_jmp;

static mu_t scope = 0;
//static mu_t args = 0;


mu_noreturn mu_sys_error(const char *m, muint_t len) {
    //printf("\x1b[31merror: %.*s\x1b[0m\n", (unsigned)len, m);
    printf("error:");
    char pr[len];
    memcpy(pr, m, len);
	printf("%s:\n",pr);
	
	longjmp(mu_error_jmp, 1);
}

static mcnt_t mu_bfn_dis(mu_t *frame) {
    if (!mu_isfn(frame[0])) {
        mu_errorf("expected mu function");
    }

    mu_t code = mu_fn_getcode(frame[0]);
    if (!code) {
        mu_errorf("expected mu function");
    }

    mu_dis(code);
    mu_dec(code);
    mu_dec(frame[0]);
    return 0;
}

MU_GEN_STR(mu_sys_key_dis, "dis")
MU_GEN_BFN(mu_sys_bfn_dis, 0x1, mu_bfn_dis)
MU_GEN_TBL(mu_sys_tbl_dis, {
    { mu_sys_key_dis, mu_sys_bfn_dis }
})

MU_GEN_TBL(mu_sys_imports, {
    { mu_sys_key_dis, mu_sys_tbl_dis }
})

mu_t mu_sys_import(mu_t name) {
    mu_t module = mu_tbl_lookup(mu_sys_imports(), name);
    return module;
}

static void init_scope(void) {
    scope = mu_tbl_extend(0, MU_BUILTINS);
}

int mu_process_buffer(char * buffer) {
  if (! strcmp("bye",buffer)) {
    return 1;
  }
  if (! strcmp("exit",buffer)) {
    thread_exit(1);
  }
  if (!setjmp(mu_error_jmp)) {
	mu_eval(buffer, strlen(buffer), scope, 0);
  }
  pos = 0;
  return 0;
}

static int muapp(int argc, const console_cmd_args *argv) {
    printf("muapp %s\r\n", "0.0");
    printf("type 'bye' to exit\r\n");

    init_scope();

    char buffer[200];
    
    int escape_level = 0;
    for (;;) {
        /* loop until we get a char */
        int c;
        if ((c = getchar()) < 0)
            continue;

        if (escape_level == 0) {
            switch (c) {
                //case 'q':
                //    goto done;
                case '\r':                    
                case '\n':
                    buffer[pos++] = '\0';
                    putchar('\n');
                    //thread_detach_and_resume(thread_create("muapp_worker", &mu_process_buffer, buffer, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
                    if ( mu_process_buffer(&buffer) == 1 ) 
                      goto done;

                case 0x7f: // backspace or delete
                case 0x8:
                    if (pos > 0) {
                        pos--;
                        fputs("\b \b", stdout); // wipe out a character
                    }
                    break;

                case 0x1b: // escape
                    escape_level++;
                    break;

                default:
                    buffer[pos++] = c;
                    
                    putchar(c);
            }
        } else if (escape_level == 1) {
            // inside an escape, look for '['
            if (c == '[') {
                escape_level++;
            } else {
                // we didn't get it, abort
                escape_level = 0;
            }
        } else { // escape_level > 1
            switch (c) {
                case 67: // right arrow
                    buffer[pos++] = ' ';
                    putchar(' ');
                    break;
                case 68: // left arrow
                    if (pos > 0) {
                        pos--;
                        fputs("\b \b", stdout); // wipe out a character
                    }
                    break;
                default:
                    break;
            }
            escape_level = 0;
        }

        /* end of line. */
        if (pos == (LINE_LEN - 1)) {
            fputs("\nerror: line too long\n", stdout);
            pos = 0;
            goto done;
        }
    }

	done:
		return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("muapp", "muapp tests", &muapp)
STATIC_COMMAND_END(muapp);

APP_START(muapp)
APP_END
