#include "kforth.h"
#include "dictionary_1.h"


#define LOOP_STACK_PUSH(x)	(*context->loop_stack_ptr++=x);
#define LOOP_STACK_POP()	(*--context->loop_stack_ptr)
#define LOOP_STACK_POP_X(a)     (context->loop_stack_ptr-=a)

#define STACK_TOP       (context->stack_ptr[-1])
#define STACK(a)        (context->stack_ptr[-1+a])
#define STACK_PUSH(x)   (*context->stack_ptr++=x)
#define STACK_POP()     (*--context->stack_ptr)
#define STACK_POP_X(a)  (context->stack_ptr-=a)


int fn_bye(_kforth_context *context)
{
	kforth_exit();
    return 0;
}

int fn_write(_kforth_context *context)
{
    platform_printf("Writing 0x%X to 0x%08X\r\n", STACK(0), STACK(-1));
    STACK_POP_X(2);
    return 0;
}

int fn_read(_kforth_context *context)
{
    platform_printf("Reading From 0x%08X\r\n", STACK(0));
    STACK_POP();
    STACK_PUSH(0);
    return 0;
}

int fn_print_top(_kforth_context *context)
{
    if (context->stack==context->stack_ptr)
    {
        platform_printf("EOS\r\n");
    }
    else
    {
        platform_printf("%d ", context->stack_ptr[-1]);
        context->stack_ptr--;
    }
    return 0;
}

int fn_print_stack(_kforth_context *context)
{
    int *p1;
    for(p1=context->stack; p1!=context->stack_ptr; p1++)
    {
        platform_printf("%d ", *p1);
    }
    platform_printf("\r\n");
    return 0;
}


int fn_print_loop_stack(_kforth_context *context)
{
    int *p1;
    for(p1=context->loop_stack; p1!=context->loop_stack_ptr; p1++)
    {
        platform_printf("%d ", *p1);
    }
    platform_printf("\r\n");
    return 0;
}

int fn_plus(_kforth_context *context)
{
    context->stack_ptr[-2]=context->stack_ptr[-2]+context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

double fn_plus_f(_kforth_context *context)
{
    context->stack_ptr[-2]=context->stack_ptr[-2]+context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

int fn_mul(_kforth_context *context)
{
    context->stack_ptr[-2]=context->stack_ptr[-2]*context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

int fn_increment(_kforth_context *context)
{
    context->stack_ptr[-1]++;
    return 0;
}

int fn_minus(_kforth_context *context)
{
    context->stack_ptr[-2]=context->stack_ptr[-2]-context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

int fn_mod(_kforth_context *context)
{
    context->stack_ptr[-2]=context->stack_ptr[-2]%context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

int fn_not_equal(_kforth_context *context)
{
	if (context->stack_ptr[-2]!=context->stack_ptr[-1])
	{
		context->stack_ptr[-2]=FORTH_TRUE;
	}
	else
	{
		context->stack_ptr[-2]=FORTH_FALSE;
	}
	context->stack_ptr--;
	return 0;
}

int fn_greater_than(_kforth_context *context)
{
	if (context->stack_ptr[-2]>context->stack_ptr[-1])
	{
		context->stack_ptr[-2]=FORTH_TRUE;
	}
	else
	{
		context->stack_ptr[-2]=FORTH_FALSE;
	}
	context->stack_ptr--;
	return 0;
}

int fn_less_than(_kforth_context *context)
{
	if (context->stack_ptr[-2]<context->stack_ptr[-1])
	{
		context->stack_ptr[-2]=FORTH_TRUE;
	}
	else
	{
		context->stack_ptr[-2]=FORTH_FALSE;
	}
	context->stack_ptr--;
	return 0;
}

int fn_greater_than_equal(_kforth_context *context)
{
	if (context->stack_ptr[-2]>=context->stack_ptr[-1])
	{
		context->stack_ptr[-2]=FORTH_TRUE;
	}
	else
	{
		context->stack_ptr[-2]=FORTH_FALSE;
	}
	context->stack_ptr--;
	return 0;
}


int fn_equal(_kforth_context *context)
{
    if (context->stack_ptr[-2]==context->stack_ptr[-1])
    {
        context->stack_ptr[-2]=FORTH_TRUE;
    }
    else
    {
        context->stack_ptr[-2]=FORTH_FALSE;
    }
    context->stack_ptr--;

    return 0;
}

int fn_rot(_kforth_context *context)
{
    context->stack_ptr[0]=context->stack_ptr[-3];
    context->stack_ptr[-3]=context->stack_ptr[-2];
    context->stack_ptr[-2]=context->stack_ptr[-1];
    context->stack_ptr[-1]=context->stack_ptr[0];
    return 0;
}

int fn_over(_kforth_context *context)
{
    context->stack_ptr[0]=context->stack_ptr[-2];
    context->stack_ptr++;
    return 0;
}

int fn_tuck(_kforth_context *context)
{
    context->stack_ptr[0]=context->stack_ptr[-1];
    context->stack_ptr[-1]=context->stack_ptr[-2];
    context->stack_ptr[-2]=context->stack_ptr[0];
    context->stack_ptr++;
    return 0;
}

int fn_max(_kforth_context *context)
{
    if (context->stack_ptr[-1]>context->stack_ptr[-2]) context->stack_ptr[-2]=context->stack_ptr[-1];
    context->stack_ptr--;
    return 0;
}

int fn_dup(_kforth_context *context)
{
    context->stack_ptr[0]=context->stack_ptr[-1];
    context->stack_ptr++;
    return 0;
}

int fn_drop(_kforth_context *context)
{
    if (context->stack_ptr!=context->stack) context->stack_ptr--;
    return 0;
}

int fn_swap(_kforth_context *context)
{
    context->stack_ptr[0]=context->stack_ptr[-2];
    context->stack_ptr[-2]=context->stack_ptr[-1];
    context->stack_ptr[-1]=context->stack_ptr[0];
    return 0;
}

int fn_pick(_kforth_context *context)
{
    context->stack_ptr[-1]=context->stack_ptr[-2-context->stack_ptr[-1]];
    return 0;
}

int fn_roll(_kforth_context *context)
{
    int i;
    context->stack_ptr[0]=context->stack_ptr[-2-context->stack_ptr[-1]]; // save bottom
    for(i=(-2-context->stack_ptr[-1]); i<-2; i++)
    {
        context->stack_ptr[i]=context->stack_ptr[i+1];
    }
    context->stack_ptr[-2]=context->stack_ptr[0];
    context->stack_ptr--;
    return 0;
}


// LABEL_TYPES automatically add their offset or something to the loop stack

// DO...LOOP
// LIMIT START DO BODY LOOP
#define DO_LOOP_LABEL1      (context->loop_stack_ptr[-3])
#define DO_LOOP_INDEX       (context->loop_stack_ptr[-2])
#define DO_LOOP_LIMIT       (context->loop_stack_ptr[-1])
#define DO_LOOP_LABEL2      (context->loop_stack_ptr[0]) // This is [0] because we remove it right after adding it so that all looping body code doesn't get affected

int fn_for_i(_kforth_context *context)
{
	STACK_PUSH(DO_LOOP_INDEX);
	return 0;
}

int fn_do(_kforth_context *context)
{
    LOOP_STACK_PUSH(STACK(0));
    LOOP_STACK_PUSH(STACK(-1));
    STACK_POP_X(2);
    return 0;
}

int fn_loop(_kforth_context *context)
{
    LOOP_STACK_POP(); // POP LABEL2, which was JUST added

    DO_LOOP_INDEX++; // Increment

    if (DO_LOOP_INDEX>=DO_LOOP_LIMIT)
    {
        LOOP_STACK_POP_X(3); // Pop Index, Limit and Label1
        return 0; // Don't skip keep moving on
    }

    return DO_LOOP_LABEL1-DO_LOOP_LABEL2;
}





// FOR THE FOR



// for ( ; a<b; a+=c)

#define FOR_LOOP_A 			(context->loop_stack_ptr[-1])
#define FOR_LOOP_B 			(context->loop_stack_ptr[-2])
#define FOR_LOOP_C 			(context->loop_stack_ptr[-3])
#define FOR_LOOP_LABEL1		(context->loop_stack_ptr[-4])
#define FOR_LOOP_LABEL2		(context->loop_stack_ptr[ 0])

int fn_for(_kforth_context *context)
{
	LOOP_STACK_PUSH(STACK_POP());
	LOOP_STACK_PUSH(STACK_POP());
	LOOP_STACK_PUSH(STACK_POP());

	return 0;
}

int fn_for_loop(_kforth_context *context)
{

	context->loop_stack_ptr--; // effectively popping LABEL2

	if (((FOR_LOOP_A<=FOR_LOOP_B)&&((FOR_LOOP_A+FOR_LOOP_C)>=FOR_LOOP_B)) ||
        ((FOR_LOOP_A>=FOR_LOOP_B)&&((FOR_LOOP_A+FOR_LOOP_C)<=FOR_LOOP_B)))
	{
		context->loop_stack_ptr-=4; // remove a, b, c and label 1
		return 0;
		
	}
	FOR_LOOP_A+=FOR_LOOP_C;
	return FOR_LOOP_LABEL1-FOR_LOOP_LABEL2;


}


int fn_for_a(_kforth_context *context)
{
	STACK_PUSH(FOR_LOOP_A);
	return 0;
}




#define DO_WHILE_LABEL1	(context->loop_stack_ptr[-1])
#define DO_WHILE_LABEL2	(context->loop_stack_ptr[ 0])

int fn_while(_kforth_context *context)
{
	context->loop_stack_ptr--; // effectively popping LABEL2

	if (STACK_POP()==FORTH_TRUE)
	{
		return DO_WHILE_LABEL1-DO_WHILE_LABEL2;
	}

	context->loop_stack_ptr--;
	return 0;
}


int fn_elif(_kforth_context *context)
{
    return STACK_POP();
}

int fn_if(_kforth_context *context)
{
    context->stack_ptr-=2;
    if (context->stack_ptr[0]==FORTH_TRUE)
    {
        return 0;
    }

    return context->stack_ptr[1];
}
    
int fn_endif(_kforth_context *context)
{
    return 0;
}


int fn_print_dictionary_full(_kforth_context *context)
{
    _word_entry_const *entry;
    _word_list_item *word_list;
    platform_printf("Print Dictionary\r\n");
    platform_printf("dictionary at 0x%08X\r\n", (unsigned int)context->dictionary);

    platform_printf("ADDRESS     NEXT        NAME     WORD_FN     WORD_LIST\r\n");

    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        platform_printf("%08X    %08X    %5s    %08X    ", (unsigned int)entry, (unsigned int)entry->next,entry->name, (unsigned int)entry->word_fn);
        if (entry->word_list!=NULL)
        {
            for(word_list=(_word_list_item*)entry->word_list; word_list->type!=END_TYPE; word_list++)
            {
                platform_printf("%X:%08X ", word_list->type, (unsigned int)word_list->data);
            }
        }
        platform_printf("\r\n");
    }
    return 0;
}

int fn_print_dictionary(_kforth_context *context)
{
    char format_str[10];
    _word_entry_const *entry;
    _word_list_item *word_list;
    int name_length;
    int names_per_line;
    int name_index;
    int i;

    // first calculate the max name length
    name_length=0;
    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        i=platform_strlen(entry->name);
        if (i>name_length)name_length=i;
    }

    // now calculate the max number of names to print per line
    names_per_line=10; // +2 for 2 spaces between names

    platform_sprintf(format_str, "%%-%ds", name_length+2); // "%x-s"

    platform_printf("Print Dictionary\r\n");
    for(entry=(_word_entry_const*)context->dictionary, name_index=1; entry!=NULL; entry=(_word_entry_const*)entry->next, name_index++)
    {
        if ((name_index>0)&&((name_index%names_per_line)==0))
        {
            platform_printf("%s\r\n", entry->name);
        }
        else
        {
            platform_printf(format_str, entry->name);
        }
    }
    platform_printf("\r\n\r\n");
    return 0;
}

int fn_save(_kforth_context *context)
{
	return 0;
}

int fn_load(_kforth_context *context)
{
	return 0;
}

int fn_reset(_kforth_context *context)
{
	return 0;
}

int fn_cr(_kforth_context *context)
{
	platform_printf("\r\n");
    return 0;
}

int fn_store(_kforth_context *context)
{
    // value = 1
	// & value 1 store
	_word_entry_ram *entry = (_word_entry_ram*)context->stack_ptr[-2];

	((_word_list_item*)entry->word_list)[0].data=(void*)context->stack_ptr[-1];
    context->stack_ptr-=2;
    return 0;
}

int fn_get(_kforth_context *context)
{
	_word_entry_ram *entry = (_word_entry_ram*)context->stack_ptr[-1];
	context->stack_ptr[-1]=(int)((_word_list_item*)entry->word_list)[0].data;
	return 0;
}

			                         //                 NEXT                            NAME        word_fn                     		word_list
const _word_entry_const entry_read	    			={  NULL,							"read",	    (void*)fn_read, 					NULL };
const _word_entry_const entry_write 				={  &entry_read,					"write",	(void*)fn_write,					NULL };
const _word_entry_const entry_for					={  &entry_write,					"for",		(void*)fn_for,						NULL };
const _word_entry_const entry_for_loop				={  &entry_for,						"forloop",	(void*)fn_for_loop,					NULL };
const _word_entry_const entry_loop					={  &entry_for_loop,				"loop",		(void*)fn_loop,						NULL };
const _word_entry_const entry_for_a					={  &entry_loop,					"A",		(void*)fn_for_a,					NULL };
const _word_entry_const entry_for_i					={  &entry_for_a,					"I",		(void*)fn_for_i,					NULL };
const _word_entry_const entry_do					={  &entry_for_i,					"do",		(void*)fn_do,						NULL };
const _word_entry_const entry_while					={  &entry_do,						"while",	(void*)fn_while,					NULL };

const _word_entry_const entry_cr                    ={  &entry_while,                   "cr",       (void*)fn_cr,                      	NULL };

const _word_entry_const entry_if           			={  &entry_cr,                      "if",  		(void*)fn_if,		             	NULL };
const _word_entry_const entry_elif                	={  &entry_if,             			"elif",   	(void*)fn_elif,                  	NULL };
const _word_entry_const entry_endif					={	&entry_elif,					"endif",	(void*)fn_endif,					NULL };

const _word_entry_const entry_equal                 ={  &entry_endif,                   "=",        (void*)fn_equal,                   	NULL };
const _word_entry_const entry_less_than				={ 	&entry_equal,					"<",		(void*)fn_less_than,				NULL };
const _word_entry_const entry_greater_than_equal	={ 	&entry_less_than,				">=",		(void*)fn_greater_than_equal,		NULL };
const _word_entry_const entry_not_equal				={ 	&entry_greater_than_equal,		"!=",		(void*)fn_not_equal,				NULL };
const _word_entry_const entry_greater_than			={  &entry_not_equal,				">",		(void*)fn_greater_than,				NULL };

const _word_entry_const entry_max                   ={  &entry_greater_than,            "max",      (void*)fn_max,                     	NULL };
const _word_entry_const entry_dup                   ={  &entry_max,                     "dup",      (void*)fn_dup,                     	NULL };
const _word_entry_const entry_drop                  ={  &entry_dup,                     "drop",     (void*)fn_drop,                    	NULL };
const _word_entry_const entry_swap                  ={  &entry_drop,                    "swap",     (void*)fn_swap,                    	NULL };
const _word_entry_const entry_rot                   ={  &entry_swap,                    "rot",      (void*)fn_rot,                     	NULL };
const _word_entry_const entry_over                  ={  &entry_rot,                     "over",     (void*)fn_over,                    	NULL };
const _word_entry_const entry_tuck                  ={  &entry_over,                    "tuck",     (void*)fn_tuck,                    	NULL };
const _word_entry_const entry_pick                  ={  &entry_tuck,                    "pick",     (void*)fn_pick,                    	NULL };
const _word_entry_const entry_roll                  ={  &entry_pick,                    "roll",     (void*)fn_roll,                    	NULL };

const _word_entry_const entry_store                 ={  &entry_roll,                    "store",    (void*)fn_store,                   	NULL };
const _word_entry_const entry_get					={ 	&entry_store,					"get",		(void*)fn_get,						NULL };

const _word_entry_const entry_minus                 ={  &entry_get,                     "-",        (void*)fn_minus,                   	NULL };
const _word_entry_const entry_plus                  ={  &entry_minus,                   "+",        (void*)fn_plus,                    	NULL };
const _word_entry_const entry_plus_f                ={  &entry_plus,                    "f+",       (void*)fn_plus_f,                   NULL };
const _word_entry_const entry_mul                   ={  &entry_plus_f,                  "*",        (void*)fn_mul,                     	NULL };
const _word_entry_const entry_increment             ={  &entry_mul,                     "1+",       (void*)fn_increment,               	NULL };

const _word_entry_const entry_print_loop_stack	    ={ 	&entry_increment,				".sl",		(void*)fn_print_loop_stack,			NULL };

const _word_entry_const entry_print_stack           ={  &entry_print_loop_stack,        ".s",       (void*)fn_print_stack,             	NULL };
const _word_entry_const entry_print_top             ={  &entry_print_stack,             ".",        (void*)fn_print_top,               	NULL };

const _word_entry_const entry_print_dictionary      ={  &entry_print_top,               "??",       (void*)fn_print_dictionary_full,   	NULL };
const _word_entry_const entry_print_dictionary_full ={  &entry_print_dictionary,        "?" ,       (void*)fn_print_dictionary,        	NULL };
const _word_entry_const entry_save 					={  &entry_print_dictionary_full,	"save",		(void*)fn_save,					   	NULL };
const _word_entry_const entry_load					={  &entry_save,					"load",		(void*)fn_load,						NULL };
const _word_entry_const entry_reset					={  &entry_load,					"reset",	(void*)fn_reset,					NULL };

const _word_entry_const entry_bye                   ={  &entry_reset, 					"bye",      (void*)fn_bye,                     	NULL };

const _word_entry_const *dictionary_1=&entry_bye;

/***   End Of File   ***/

