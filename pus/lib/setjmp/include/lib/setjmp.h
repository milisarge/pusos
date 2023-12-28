/*
https://gcc.gnu.org/onlinedocs/gcc/Nonlocal-Gotos.html
https://nullprogram.com/blog/2023/02/12/
*/

#ifndef SETJMP_H
#define SETJMP_H

typedef void *jmp_buf[5];

#define setjmp __builtin_setjmp
#define longjmp __builtin_longjmp

#endif /* _SETJMP_H_ */
