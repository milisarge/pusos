#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>
#define LINE_LEN 128

static int pos = 0;

#include "jl.h"

struct JLContext *gcontext;

static struct JLValue *PrintFunc(struct JLContext *context,
                                 struct JLValue *args,
                                 void *extra)
{
   struct JLValue *vp;
   for(vp = JLGetNext(args); vp; vp = JLGetNext(vp)) {
      struct JLValue *result = JLEvaluate(context, vp);
      if(JLIsString(result)) {
         printf("%s", JLGetString(result));
      } else {
         JLPrint(context, result);
      }
      JLRelease(context, result);
   }
   return NULL;
}

static struct JLValue *ProcessBuffer(struct JLContext *context,
                                     const char *line)
{
   struct JLValue *result = NULL;
   
   while(*line) {
      struct JLValue *value = JLParse(context, &line);
      if(value) {
         JLRelease(context, result);
         result = JLEvaluate(context, value);
         JLRelease(context, value);
      }
   }
   return result;
}

static int process_buffer(char * buffer) {
	
	if (strcmp(buffer,"exit") == 0)
	  return 1;
	
	struct JLValue *result;
	JLDefineSpecial(gcontext, "print", PrintFunc, NULL);
	result = ProcessBuffer(gcontext, buffer);
	printf("=> ");
	JLPrint(gcontext, result);
	printf("\n");
	JLRelease(gcontext, result);
	pos = 0;
	return 0;
}


static int jli(int argc, const console_cmd_args *argv) {

	gcontext = JLCreateContext();
   
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
		JLDestroyContext(gcontext);
		return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("lisp", "lisp tests", &jli)
STATIC_COMMAND_END(lisp);

APP_START(lisp)
APP_END
