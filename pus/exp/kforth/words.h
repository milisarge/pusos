#ifndef WORDS_H
#define WORDS_H

// void * is of type (_kforth_context), declared as void* since words.h needs to come before kforth.h
typedef int (*_word_fn)(void *context);

typedef struct
{
    #define END_TYPE        0x12
    #define LITERAL_TYPE    0x13
    #define WORD_TYPE       0x14
	#define STR_TYPE	    0x15
	#define LABEL_TYPE	    0x16
	#define BRANCH_TYPE	    0x17
    #define RECURSIVE_TYPE  0x18
    char type;
    void *data;
}_word_list_item;

typedef struct
{
    const void            *next;
    const char            *name;
    const _word_fn        word_fn;
    const void            *word_list;
}_word_entry_const;

typedef struct
{
    void            *next;
    char            *name;
    _word_fn        word_fn;
    void            *word_list;
}_word_entry_ram;



#endif
/***   End Of File   ***/

