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

#include "pickle.h"


typedef struct { long allocs, frees, reallocs, total; uint64_t tick, after; } heap_t;

/* NB. This allocator allows us to test failure modes of the interpreter */
static void *allocator(void *arena, void *ptr, const size_t oldsz, const size_t newsz) {
	/* assert(h && (h->frees <= h->allocs)); */
	heap_t *h = arena;
	const int fail = h->after && h->tick++ > h->after;
	if (newsz == 0) { if (ptr) h->frees++; free(ptr); return fail ? arena : NULL; }
	if (newsz > oldsz) { h->reallocs += !!ptr; h->allocs++; h->total += newsz; return fail ? NULL : realloc(ptr, newsz); }
	return ptr;
}

static int release(pickle_t *i, void *ptr) {
	void *arena = NULL;
	allocator_fn fn = NULL;
	const int r1 = pickle_allocator_get(i, &fn, &arena);
	if (fn)
		fn(arena, ptr, 0, 0);
	return fn ? r1 : PICKLE_ERROR;
}


static int process_buffer(char * buffer) {
  //printf("\ncode->\"%s\" (%ld)\n",buffer,strlen(buffer));	
  //printf("kod:%s:%d:",buffer,pos);
  heap_t h = { 0, 0, 0, 0, 0, 0, };
  pickle_t *i = NULL;
  //if (pickle_tests(allocator, &h)   != PICKLE_OK) return 1;
  pickle_new(&i, allocator, &h);
  const int r = pickle_eval(i, buffer);
  const char *s = NULL;
  if (r == 1) {
    return 1;
  } else {
	pickle_result_get(&i,&s);
    printf("%s",s);
    printf("ok\r\n");
  }
  
  //return release(i, buffer) == PICKLE_OK ? r : PICKLE_ERROR;

  pos = 0;
  return 0;
}

static int pickle(int argc, const console_cmd_args *argv) {
    
    printf("type 'bye' to exit\r\n");

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
STATIC_COMMAND("pickle", "pickle tests", &pickle)
STATIC_COMMAND_END(pickle);

APP_START(pickle)
APP_END
