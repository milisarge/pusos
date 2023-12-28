#include <stdio.h>
#include <string.h>
#include <app.h>
#include <lk/debug.h>
#include <lib/console.h>

#define LINE_LEN 32

static void process_buffer(char * buffer) {
  printf("buffer:%s:%ld\n",buffer,strlen(buffer));	
}

static void myforth(const struct app_descriptor *app, void *args){
    char buffer[100];
     int pos = 0;
    int escape_level = 0;
    for (;;) {
        /* loop until we get a char */
        int c;
        if ((c = getchar()) < 0)
            continue;

//      TRACEF("c = 0x%hhx\n", c);

        if (escape_level == 0) {
            switch (c) {
                case 'q':
                    goto done;

                case '\r':                    
                case '\n':
                    buffer[pos++] = '\n';
                    putchar('\n');
                    //goto done;

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
    buffer[pos] = 0;
    
    process_buffer(&buffer);
    return;
}

STATIC_COMMAND_START
STATIC_COMMAND("myforth", "myforth tests", &myforth)
STATIC_COMMAND_END(myforth);

APP_START(myforth)
APP_END
