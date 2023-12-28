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

#include "kforth.h"
#include "dictionary_1.h"

_kforth_context test_context;

int test_stack[1000];
int test_loop_stack[100];
char statement[200];

static int process_buffer(char * buffer) {
  //printf("\ncode->\"%s\" (%ld)\n",buffer,strlen(buffer));	
  //printf("kod:%s:%d:",buffer,pos);
  int ret=kforth_execute_statement(&test_context, buffer);
  if (ret!=0) {
    //printf("Exiting with %d\r\n", ret);
    return 1;
  } 
  else {
    printf("ok\r\n");
  }
  pos = 0;
  return 0;
}

static int kforth(int argc, const console_cmd_args *argv) {
    printf("kforth %s\r\n", KFORTH_VERSION);
    printf("type 'bye' to exit\r\n");

    kforth_init_context(&test_context, test_stack, test_loop_stack, dictionary_1 );

    char buffer[200];
    
    int escape_level = 0;
    for (;;) {
        /* loop until we get a char */
        int c;
        if ((c = getchar()) < 0)
            continue;

//      TRACEF("c = 0x%hhx\n", c);

        if (escape_level == 0) {
            switch (c) {
                //case 'q':
                //    goto done;
                case '\r':                    
                case '\n':
                    buffer[pos++] = '\0';
                    putchar('\n');
                    if ( process_buffer(&buffer) == 1 ) 
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
	//  dprintf("returning pos %d\n", pos);

		// null terminate
		//buffer[pos] = 0;
		
		//process_buffer(&buffer);
		return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("kforth", "kforth tests", &kforth)
STATIC_COMMAND_END(kforth);

APP_START(kforth)
APP_END
