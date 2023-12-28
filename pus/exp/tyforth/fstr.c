/*
 * This file is part of arm-sim: http://madscientistroom.org/arm-sim
 *
 * Copyright (c) 2010 Randy Thelen. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include "forth.h"
#include "fobj.h"

void fstr_print(fenv_t *f, fobj_t *p)
{
    char *s = p->u.str.buf ? p->u.str.buf : "(null)";

#ifdef DEBUG
    printf("    String = %s\n", s);
#else
    printf("%s", s);
#endif
}

fobj_t *fstr_new_buf(fenv_t *f, const char *buf, int len)
{
    fobj_t *p = fobj_new(f, FOBJ_STR);
    p->u.str.len = len;
    p->u.str.buf = malloc(len + 1);
    bcopy(buf, p->u.str.buf, len);
    p->u.str.buf[len] = 0;
    return p;
}

fobj_t *fstr_new(fenv_t *f, const char *str)
{
    fobj_t *p = fobj_new(f, FOBJ_STR);
    p->u.str.len = strlen(str);
    p->u.str.buf = malloc(p->u.str.len + 1);
    p->u.str.buf[0] = 0;
    strcat(p->u.str.buf, str);
    return p;
}

void fstr_free(fenv_t *f, fobj_t *p)
{
    if (p->u.str.buf) {
        free(p->u.str.buf);
        p->u.str.buf = NULL;
    }
    p->u.str.len = 0;
}

/*
 * fstr_add()
 *
 * Supported operand types (for op2):
 *    - String: concatenate the two strings
 *    - Number: index into string
 */

static fobj_t *fstr_concatenate(fenv_t *f, fobj_t *op1, fobj_t *op2)
{
    fobj_t *dest = fstr_new(f, op1->u.str.buf);
    dest->u.str.len += op2->u.str.len;
    dest->u.str.buf = realloc(dest->u.str.buf, dest->u.str.len + 1);
    strcat(dest->u.str.buf, op2->u.str.buf);
    return dest;
}

static fobj_t *fstr_compare(fenv_t *f, fobj_t *op1, fobj_t *op2)
{
    int r = strcmp(op1->u.str.buf, op2->u.str.buf);
    return fnum_new(f, r);
}

fobj_t *fstr_add(fenv_t *f, fobj_t *op1, fobj_t *op2)
{
    ASSERT(op1->type == FOBJ_STR);
    FASSERT(op2->type == FOBJ_STR, "Wrong type");
    return fstr_concatenate(f, op1, op2);
}

/*
 * fstr_sub()
 *
 * Supported operand types (for op2):
 *    - String: compare the two strings (leave a number)
 *    - Number: index into string
 */

fobj_t *fstr_sub(fenv_t *f, fobj_t *op1, fobj_t *op2)
{
    ASSERT(op1->type == FOBJ_STR);
    FASSERT(op2->type == FOBJ_STR, "Wrong type");
    return fstr_compare(f, op1, op2);
}

int fstr_cmp(fenv_t *f, fobj_t *a, fobj_t *b)
{
    if (a == b) return 0;

    fstr_t *A = &a->u.str;
    fstr_t *B = &b->u.str;
    int cmp;

    if (A->len < B->len) {
        cmp = memcmp(A->buf, B->buf, A->len);
        if (cmp == 0) return 1; // B is 'bigger'
        else          return cmp;
    } else if (A->len > B->len) {
        cmp = memcmp(A->buf, B->buf, B->len);
        if (cmp == 0) return -1; // A is 'bigger'
        else          return cmp;
    } else {
        cmp = memcmp(A->buf, B->buf, B->len);
        return cmp;
    }
}

fobj_t *fstr_fetch(fenv_t *f, fobj_t *addr, fobj_t *index)
{
    // fstr_t *addr = &op1->u.str;

    return 0;
}

int fstr_len(fenv_t *f, fobj_t *str)
{
    FASSERT(str->type == FOBJ_STR, "STRING required");
    return str->u.str.len;
}

int fstr_getchar(fenv_t *f, fobj_t *str, int offset)
{
    if (offset < str->u.str.len) {
        return str->u.str.buf[offset];
    } else {
        return EOF;
    }
}

fnumber_t fstr_to_number(fenv_t *f, fobj_t *str)
{
    return strtold(str->u.str.buf, NULL);
}
