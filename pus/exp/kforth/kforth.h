#ifndef KFORTH_H
#define KFORTH_H

#define KFORTH_VERSION "v0.1"

#define FORTH_TRUE (-1)
#define FORTH_FALSE (0)

// Declare NULL as 0 for kforth functions, this is normally defined in stdlib, stdio, string, etc
#ifndef NULL
#define NULL ((void*)0L)
#endif

#include "platform_lk.h"
#include "words.h"

typedef struct
{
    int *stack;
    int *stack_ptr;
    int *loop_stack;
    int *loop_stack_ptr;
	_word_entry_ram *dictionary;
	_word_entry_ram *saved_dictionary;
}_kforth_context;



void kforth_init_context(_kforth_context *context, int *stack_in, int *loop_stack_in, const _word_entry_const *dictionary_in);
int kforth_execute_statement(_kforth_context *context, char *statement);
void kforth_exit(void);

#endif
/***   End Of File   ***/
