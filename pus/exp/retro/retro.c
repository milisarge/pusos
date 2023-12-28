/* RETRO is a clean, elegant, and pragmatic dialect of Forth.
   It provides a simple alternative for those willing to make a
   break from legacy systems.

   The language draws influences from many sources including
   traditional Forth systems, cmForth, colorForth, Factor, and
   Parable. It was designed to be easy to grasp and adapt to
   specific uses.

   The basic language is very portable and runs on a tiny
   virtual machine.

   This file contains a minimal implementation of the virtual
   machine.

   Copyright (c) 2008 - 2022, Charles Childers
   Copyright (c) 2009 - 2010, Luke Parrish
   Copyright (c) 2010,        Marc Simpson
   Copyright (c) 2010,        Jay Skeer
   Copyright (c) 2011,        Kenneth Keating
*/

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "image.h"
#include <app.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#define CELL int64_t
#define CELL_MIN LLONG_MIN + 1
#define CELL_MAX LLONG_MAX - 1

#ifndef IMAGE_SIZE
#define IMAGE_SIZE   524288       /* Amount of RAM, in cells */
#endif

#ifndef ADDRESSES
#define ADDRESSES    256          /* Depth of address stack */
#endif

#ifndef STACK_DEPTH
#define STACK_DEPTH  256          /* Depth of data stack */
#endif

CELL sp, rp, ip;                  /* Stack and instruction pointers */
CELL data[STACK_DEPTH];           /* The data stack          */
CELL address[ADDRESSES];          /* The address stack       */
CELL memory[IMAGE_SIZE + 1];      /* Image Memory            */

#define TOS  data[sp]             /* Top item on stack       */
#define NOS  data[sp-1]           /* Second item on stack    */
#define TORS address[rp]          /* Top item on address stack */

typedef void (*Handler)(void);

#define LINE_LEN 64

static int pos = 0;

static int exit_status = 0;
char *retro_code;
//char retro_code[128];

CELL stack_pop();
void stack_push(CELL value);
void execute(CELL cell);
void load_image(char *imageFile);
//void save_image();
void process_opcode_bundle(CELL opcode);

CELL stack_pop() {
  sp--;
  return data[sp + 1];
}

void stack_push(CELL value) {
  sp++;
  data[sp] = value;
}

void execute(CELL cell) {
  CELL opcode;
  rp = 1;
  ip = cell;
  while (ip < IMAGE_SIZE) {
    opcode = memory[ip];
    process_opcode_bundle(opcode);
    ip++;
    if (rp == 0)
      ip = IMAGE_SIZE;
  }
}

void load_embedded_image() {
  int i;
  for (i = 0; i < ngaImageCells; i++)
    memory[i] = ngaImage[i];
}

CELL string_inject(char *str, CELL buffer) {
  CELL m, i;
  if (!str) {
    memory[buffer] = 0;
    return 0;
  }
  m = strlen(str);
  i = 0;
  while (m > 0) {
    memory[buffer + i] = (CELL)str[i];
    memory[buffer + i + 1] = 0;
    m--; i++;
  }
  return buffer;
}

void evaluate(char *s) {
  if (strlen(s) == 0)  return;
  string_inject(s, 120);
  stack_push(120);
  execute(0);
}

static int retro_main(int argc, const console_cmd_args *argv) {
  //load_image("retro.snapshot");
  load_embedded_image();
  
  //retro_code = "#1 #2 + n:put nl 'hello! s:put nl ";
  //evaluate(retro_code);
  
  //execute(0);
  thread_detach_and_resume(thread_create("retro_processor", &execute, 0, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  
  //save_image();
  return 0;
}


void inst_no() {
}

void inst_li() {
  ip++;
  stack_push(memory[ip]);
}

void inst_du() {
  stack_push(TOS);
}

void inst_dr() {
  data[sp] = 0;
   if (--sp < 0)
     ip = IMAGE_SIZE;
}

void inst_sw() {
  CELL a;
  a = TOS;
  TOS = NOS;
  NOS = a;
}

void inst_pu() {
  rp++;
  TORS = TOS;
  inst_dr();
}

void inst_po() {
  stack_push(TORS);
  rp--;
}

void inst_ju() {
  ip = TOS - 1;
  inst_dr();
}

void inst_ca() {
  rp++;
  TORS = ip;
  ip = TOS - 1;
  inst_dr();
}

void inst_cc() {
  CELL a, b;
  a = TOS; inst_dr();  /* Target */
  b = TOS; inst_dr();  /* Flag   */
  if (b != 0) {
    rp++;
    TORS = ip;
    ip = a - 1;
  }
}

void inst_re() {
  ip = TORS;
  rp--;
}

void inst_eq() {
  NOS = (NOS == TOS) ? -1 : 0;
  inst_dr();
}

void inst_ne() {
  NOS = (NOS != TOS) ? -1 : 0;
  inst_dr();
}

void inst_lt() {
  NOS = (NOS < TOS) ? -1 : 0;
  inst_dr();
}

void inst_gt() {
  NOS = (NOS > TOS) ? -1 : 0;
  inst_dr();
}

void inst_fe() {
  switch (TOS) {
    case -1: TOS = sp - 1; break;
    case -2: TOS = rp; break;
    case -3: TOS = IMAGE_SIZE; break;
    case -4: TOS = CELL_MIN; break;
    case -5: TOS = CELL_MAX; break;
    default: TOS = memory[TOS]; break;
  }
}

void inst_st() {
  if (TOS <= IMAGE_SIZE && TOS >= 0) {
    memory[TOS] = NOS;
    inst_dr();
    inst_dr();
  } else {
    ip = IMAGE_SIZE;
  }
}

void inst_ad() {
  NOS += TOS;
  inst_dr();
}

void inst_su() {
  NOS -= TOS;
  inst_dr();
}

void inst_mu() {
  NOS *= TOS;
  inst_dr();
}

void inst_di() {
  CELL a, b;
  a = TOS;
  b = NOS;
  TOS = b / a;
  NOS = b % a;
}

void inst_an() {
  NOS = TOS & NOS;
  inst_dr();
}

void inst_or() {
  NOS = TOS | NOS;
  inst_dr();
}

void inst_xo() {
  NOS = TOS ^ NOS;
  inst_dr();
}

void inst_sh() {
  CELL y = TOS;
  CELL x = NOS;
  if (TOS < 0)
    NOS = NOS << (0 - TOS);
  else {
    if (x < 0 && y > 0)
      NOS = x >> y | ~(~0U >> y);
    else
      NOS = x >> y;
  }
  inst_dr();
}

void inst_zr() {
  if (TOS == 0) {
    inst_dr();
    ip = TORS;
    rp--;
  }
}

void inst_ha() {
  ip = IMAGE_SIZE;
}

void inst_ie() {
  stack_push(2);
}

void inst_iq() {
  if (TOS == 0) {
    inst_dr();
    stack_push(0);
    stack_push(0);
  } else if (TOS == 1) {
    inst_dr();
    stack_push(1);
    stack_push(1);
  }
}

void inst_ii() {
  int c;
  if (TOS == 0) {
    inst_dr();
    putc(stack_pop(), stdout);
  } else if (TOS == 1) {
    c = retro_code[0];
    if (c <= 0) {
	  thread_exit(0);
	}
    inst_dr();
    stack_push(c);
    (void)*retro_code++;
  } else {
    inst_dr();
  }
  
}

Handler instructions[] = {
  inst_no, inst_li, inst_du, inst_dr, inst_sw, inst_pu, inst_po,
  inst_ju, inst_ca, inst_cc, inst_re, inst_eq, inst_ne, inst_lt,
  inst_gt, inst_fe, inst_st, inst_ad, inst_su, inst_mu, inst_di,
  inst_an, inst_or, inst_xo, inst_sh, inst_zr, inst_ha, inst_ie,
  inst_iq, inst_ii
};

void process_opcode_bundle(CELL opcode) {
  instructions[opcode & 0xFF]();
  instructions[(opcode >> 8) & 0xFF]();
  instructions[(opcode >> 16) & 0xFF]();
  instructions[(opcode >> 24) & 0xFF]();
}

static int process_buffer(char * buffer) {
  
  if (! strcmp("bye",buffer)) {
    return 1;
  }
  char ret[100];
  memcpy(ret, buffer, strlen(buffer));
  retro_code = ret;
  //retro_code = "#1 #2 + n:put nl 'hello! s:put nl ";
  thread_detach_and_resume(thread_create("retro_processor", &execute, 0, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
  //execute(0);
  printf("\n");
  pos = 0;
  return 0;
}

static int retro_app(int argc, const console_cmd_args *argv) {
    printf("retro %s\r\n", "0.0");
    printf("type 'bye' to exit\r\n");

    load_embedded_image();

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
                    //thread_detach_and_resume(thread_create("retro_process", &process_buffer, buffer, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
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
STATIC_COMMAND("retro", "retro tests", &retro_app)
STATIC_COMMAND_END(retro);

APP_START(retro)
APP_END
