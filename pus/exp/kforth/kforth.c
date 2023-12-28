#include "kforth.h"
#include <lib/stringx.h>

void kforth_init_context(_kforth_context *context, int *stack_in, int *loop_stack_in, const _word_entry_const *dictionary_in)
{
	context->stack=stack_in;
    context->stack_ptr=context->stack;

	context->loop_stack=loop_stack_in;
    context->loop_stack_ptr=context->loop_stack;

	context->dictionary=(_word_entry_ram*)dictionary_in;
	context->saved_dictionary=(_word_entry_ram*)dictionary_in;
}

char *get_token(char *statement)
{
    static char *p1;
    static char *p2;

    if (statement!=NULL)
    {
        p1=statement;
        p2=statement;
    }

    p1=p2;

    if (*p1=='\0')
    {
        return NULL;
    }
    else if (p1[0]=='"')// strncmp(p1, "\"", 2)==0)
    {
        //       0123
        // have: "START"
        //p1++;
        //p1[0]='"';

        p2=p1+1;

        while( p2[0]!='\0' )
        {
            if ((p2[0]=='"')&&(p2[-1]!='\\')) // if we found a quote and the previous value wasn't an escape
            {
                break;
            }
            p2++;
        }

        if (p2[0]=='\0')
        {
            platform_printerror("<<ERROR: found open quote with no close quote>>\r\n");
            return NULL;
        }

        p2[0]='\0'; // null out last double quote

        if (p2[1]=='\0') // string was last item in list, no space to remove
        {
            p2++;
            return p1;
        }

        p2[1]='\0';     // string was not the last item, we have a space to remove
        p2+=2;

        return p1;
    }
    else
    {
        for( ; *p2!='\0'; p2++)
        {
            if (*p2==' ')
            {
                *p2='\0';
                p2++;
                break;
            }
        }
        return p1;
    }
}

int is_int(char *token)
{
    if (*token=='-') token++; // only allow negatives at the start
    for( ; *token!='\0'; token++)
    {
        if ( (*token<'0')||(*token>'9')) return 0;
    }
    return 1;
}

int is_float(char *token)
{
    int point_find = 0 ; 
    if (*token=='-') token++; // only allow negatives at the start
    for( ; *token!='\0'; token++)
    {
        //if ( (*token<'0')||(*token>'9')) return 0;
        if ((*token == '.') && (point_find == 0) ) {
			point_find = 1;
		}
    }
    if (point_find == 0) return 0;
    return 1;
}
  
int is_str(char *token)
{
    if (token[0]=='"') return 1;
    return 0;
}



void push_int(_kforth_context *context, int item)
{
    *context->stack_ptr++=item;
}


int pop_int(_kforth_context *context)
{
    return *--context->stack_ptr;
}

static int exit_flag=0;

void kforth_exit(void)
{
	exit_flag=1;
}



int kforth_print_str(_kforth_context *context, char *string)
{
	int ret;
    
	platform_printf("%s",string);
	//platform_flush();

	ret=((strlen(string))+1)/sizeof(_word_list_item);
	if (((strlen(string))+1)%sizeof(_word_list_item)) ret++;

    return ret;
}

char *get_name_by_word(_kforth_context *context, void *word)
{
    _word_entry_const *entry;
    
    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        if (word==entry) return (char*)entry->name;
    }
    return "<<ERRROR WORD NOT FOUND>> ";
}

int execute_word(_kforth_context *context, _word_entry_const *entry)
{
    _word_list_item *word_list;

    if (entry->word_fn!=NULL)
    {
        return entry->word_fn(context);
    }
    else
    {
        for(word_list=(_word_list_item*)entry->word_list; (exit_flag==0)&&(word_list->type!=END_TYPE); word_list++)
        {
            switch(word_list->type)
            {
                case LITERAL_TYPE:
                    push_int(context, (int)word_list->data);
                    break;
                case WORD_TYPE:
                    word_list+=execute_word(context, word_list->data);
                    break;
				case STR_TYPE:
					word_list+=kforth_print_str(context, (char*)(word_list+1));
					break;
				case LABEL_TYPE:
					context->loop_stack_ptr[0]=(((unsigned int)word_list-(unsigned int)entry->word_list)/sizeof(_word_list_item));
					context->loop_stack_ptr++;
					break;
				case BRANCH_TYPE:
					push_int(context, (int)word_list->data);
					break;
                case RECURSIVE_TYPE:
                    word_list+=execute_word(context, entry);
                    break;
                default:
                    platform_printerror("<<ERROR Failed to execute word '%s', data type = %X>>\r\n", entry->name, word_list->type);
                    return -3;
            }
        }
    }

    return 0;
}

void kforth_see(_kforth_context *context, char *word)
{
    _word_entry_const *entry;
    _word_list_item *word_list;
	int temp;
    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        if (strcmp(entry->name, word)==0)
        {
            if (entry->word_fn!=NULL)
            {
                platform_printf("PRIMITIVE\r\n");
            }
            else if (entry->word_list!=NULL)
            {
                for(word_list=(_word_list_item*)entry->word_list; word_list->type!=END_TYPE; word_list++)
                {
                    switch(word_list->type)
                    {
                        case LITERAL_TYPE:
                            platform_printf("%d ", (int)word_list->data);
                            break;
                        case WORD_TYPE:
                            platform_printf("%s ", get_name_by_word(context, word_list->data));
                            break;
						case STR_TYPE:
							platform_printf("\"");
							word_list+=kforth_print_str(context, (char*)(word_list+1));
							platform_printf("\" ");
							break;
						case LABEL_TYPE:
//							platform_printf("LABEL_%d ", (((unsigned int)word_list-(unsigned int)entry->word_list)/sizeof(_word_list_item)));
							break;
						case BRANCH_TYPE:
//							platform_printf("BRANCH_%d ", (int)word_list->data);
							break;
                        case RECURSIVE_TYPE:
                            platform_printf("%s ", entry->name);
                            break;
                        default:
                            platform_printf("UNKNOWN TYPE %X ", word_list->type);
                            break;
                    }
                }
            }
            else
            {
                platform_printf("EMPTY WORD!\r\n");
            }
            break;
        }
    }
	platform_printf("\r\n");
}




void kforth_reset(_kforth_context *context)
{
	_word_entry_const *entry;
	_word_entry_const *next_entry;
	for(entry=(_word_entry_const*)context->dictionary; (entry !=NULL)&&(entry->word_fn==NULL); )
	{
		next_entry=(_word_entry_const*)entry->next;
		platform_free(entry);
		entry=next_entry;
	}
	context->dictionary=context->saved_dictionary;
}


int kforth_execute_word(_kforth_context *context, char *word)
{
    _word_entry_const *entry;
    int ret;

    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        if (strcmp(entry->name, word)==0)
        {
            return execute_word(context, entry);
        }
    }
    return -3;
}

void *get_word_by_name(_kforth_context *context, char *name)
{
    _word_entry_const *entry;

    for(entry=(_word_entry_const*)context->dictionary; entry!=NULL; entry=(_word_entry_const*)entry->next)
    {
        if (strcmp(name, entry->name)==0) return entry;
    }

    return NULL;
}



#define MAX_COMPILE_WORDS 100
_word_list_item new_word_list[MAX_COMPILE_WORDS];
_word_entry_ram temp_word;

_word_entry_ram *kforth_compile_word(_kforth_context *context, char *name, char *statement, int *word_list_index)
{
    char *token;
    unsigned int temp;

	*word_list_index=0;

    for(token=get_token(statement); token!=NULL; token=get_token(NULL))
    {
        if (strcmp(token, "for")==0)
		{
			// Add label
			new_word_list[(*word_list_index)].type=LABEL_TYPE;
			(*word_list_index)++;
		}
        else if (strcmp(token, "forloop")==0)
		{
			// Add label
			new_word_list[(*word_list_index)].type=LABEL_TYPE;
			(*word_list_index)++;
		}
		else if (strcmp(token, "loop")==0)
		{
			// Add label
			new_word_list[(*word_list_index)].type=LABEL_TYPE;
			(*word_list_index)++;
		}
		else if (strcmp(token, "do")==0)
		{
			// Add label
			new_word_list[(*word_list_index)].type=LABEL_TYPE;
			(*word_list_index)++;
		}
		else if (strcmp(token, "while")==0)
		{
			// Add label
			new_word_list[(*word_list_index)].type=LABEL_TYPE;
			(*word_list_index)++;
		}
        else if (strcmp(token, "if")==0)
        {
            // Add Branch
            new_word_list[(*word_list_index)].data=(void*)0;
            new_word_list[(*word_list_index)].type=BRANCH_TYPE;
            push_int(context, (*word_list_index));
            (*word_list_index)++;
        }
        else if (strcmp(token, "elif")==0)
        {
            // We need to back fill the branch literal from the previous 'if'
            // the word index of the data is stored on the stack
            temp=pop_int(context);
            new_word_list[temp].data=(void*)((*word_list_index)-temp);

            // Now add literal for branch and push index onto stack, next 'then' will fill in value
            new_word_list[(*word_list_index)].data=(void*)0;
            new_word_list[(*word_list_index)].type=BRANCH_TYPE;
            push_int(context, (*word_list_index));
            (*word_list_index)++;
        }
        else if (strcmp(token, "endif")==0)
        {
            // We need to back fill the branch literal from the previous 'if' or 'else'
            // the word index of the data is sotred on the stack
            temp=pop_int(context);
            new_word_list[temp].data=(void*)((*word_list_index)-temp-2);
        }

        if (strcmp(token, name)==0)
        {
            new_word_list[(*word_list_index)].type=RECURSIVE_TYPE;
            (*word_list_index)++;
        }
		else if (strcmp(token, "&")==0)
		{
			// this performs a word lookup of the next token and makes a word literal such that
			// the address of that word is put on the stack
			new_word_list[(*word_list_index)].data=get_word_by_name(context, get_token(NULL));
			new_word_list[(*word_list_index)].type=LITERAL_TYPE;
			(*word_list_index)++;
		}
        else if (strcmp(token, ";")==0)
        {
            // just drop it
        }
        else
        {
            // see if we have the word in the dictionary
            new_word_list[(*word_list_index)].data=get_word_by_name(context, token);
        
            if (new_word_list[(*word_list_index)].data!=NULL) // found word
            {
                new_word_list[(*word_list_index)].type=WORD_TYPE;
                (*word_list_index)++;
            }
            else if (is_int(token)==1) // found int
            {
                new_word_list[(*word_list_index)].data=(void*)atoi(token);
                new_word_list[(*word_list_index)].type=LITERAL_TYPE;
                (*word_list_index)++;
            }
            else if (is_float(token)==1) // found float
            {
                //char *end;
                double ret = atof(token);
                
                new_word_list[(*word_list_index)].data=(void*)&ret;
                new_word_list[(*word_list_index)].type=LITERAL_TYPE;
                (*word_list_index)++;
            }
            else if (is_str(token)==1) // found string
            {
                temp=platform_strlen(token);

				new_word_list[(*word_list_index)].data=0;
                new_word_list[(*word_list_index)].type=STR_TYPE;
				(*word_list_index)++;

				token++;

                platform_strcpy(((char*)&new_word_list[(*word_list_index)]), token);

				temp=((strlen(token)+1)/sizeof(_word_list_item));
				if ((strlen(token)+1)%sizeof(_word_list_item)) temp++;

                (*word_list_index)+=temp;
            }
            else
            {
                platform_printerror("<<ERROR unknown word '%s'>>\r\n", token);
            }
        }
    }

	// this signifies the end, we will continue processing though but keep track of where the end was put
    new_word_list[(*word_list_index)].type=END_TYPE;
    (*word_list_index)++;
    
	temp_word.word_fn=NULL;
	temp_word.word_list=new_word_list;

	return &temp_word;
}

void store_word(_kforth_context *context, _word_entry_ram *temp_word, int word_list_len)
{
	_word_entry_ram *new_word;
    _word_entry_ram *old_word;
    _word_entry_const *temp_entry;
    _word_list_item *word_list;

    // Now allocate memory for new word
    new_word=platform_malloc( (sizeof(_word_entry_ram)) +                     // base structure
                              (sizeof(_word_list_item)*word_list_len) +       // room for all of the words
                              (platform_strlen(temp_word->name)+1) );         // room for the name

    // Check if word already exists
    if ((old_word = get_word_by_name(context, temp_word->name))!=NULL)
    {
        // We need to update all references to this old word to the new word
        for(temp_entry=(_word_entry_const*)context->dictionary; temp_entry!=NULL; temp_entry=(_word_entry_const*)temp_entry->next)
        {
            if (temp_entry->word_fn==NULL) // RAM WORD
            {
                for(word_list=(_word_list_item*)temp_entry->word_list; word_list->type!=END_TYPE; word_list++)
                {
                    if ((word_list->type==WORD_TYPE)&&(word_list->data==old_word)) word_list->data=new_word;
                }
            }
        }

        if (old_word->word_fn==NULL) // RAM WORD
        {
            // Remove old word
            // need to find the word BEFORE this one

            if (context->dictionary==old_word) // the first one
            {
                context->dictionary = old_word->next;
            }
            else
            {
                for(temp_entry=(_word_entry_const*)context->dictionary; temp_entry!=NULL; temp_entry=(_word_entry_const*)temp_entry->next)
                {
                    if (temp_entry->next==old_word)
                    {
                        temp_entry->next = old_word->next;
                        break;
                    }
                }
            }

            free(old_word);

        }
    }

    // memory layout is as follows
    // _word_entry_ram
    // _word_list_item...all of them here
    // name
    //
    // We put name at the bottom so that each _word_list_item falls on a correct boundary

    new_word->word_list = (void*)((unsigned char*)new_word)+sizeof(_word_entry_ram);
    new_word->name      = ((unsigned char*)new_word)+sizeof(_word_entry_ram)+((word_list_len)*sizeof(_word_list_item));
    platform_strcpy(new_word->name, temp_word->name);
    platform_memcpy(new_word->word_list, temp_word->word_list, word_list_len*sizeof(_word_list_item));
    
    // Now add the new word to the dictionary
    new_word->next=context->dictionary;
    context->dictionary=new_word;
}



int kforth_execute_statement(_kforth_context *context, char *statement)
{
    char *p1;
    int ret;
	int word_list_index;
	_word_entry_ram *temp_word;

/*    platform_printf("PARSING\r\n");
    for(p1=get_token(statement); p1!=NULL; p1=get_token(NULL))
    {
        platform_printf("token[%s]\r\n", p1);
    }
    platform_printf("DONE PARSING\r\n");*/

    if (strncmp(statement, ": ", 2)==0) // compile argument
    {
		p1=get_token(statement); // get ': '
		p1=get_token(NULL);		 // get name
		statement=p1+platform_strlen(p1)+1;
	
		temp_word = kforth_compile_word(context, p1, statement, &word_list_index);
		temp_word->name=p1;
		store_word(context, temp_word, word_list_index);
		platform_printf("compiled %s\r\n", temp_word->name);
    }
    else if (strncmp(statement, "see ", 4)==0)
    {
        kforth_see(context, statement+4);
    }
	else if (strncmp(statement, "reset", 5)==0)
	{
		kforth_reset(context);
		return exit_flag;
	}
    else
    {
		temp_word=kforth_compile_word(context, "", statement, &word_list_index);
		execute_word(context, (_word_entry_const*)temp_word);
		return exit_flag;
    }
    return 0;
}


/***   End Of File ***/
