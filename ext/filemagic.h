#ifndef _FILEMAGIC_H
#define _FILEMAGIC_H

#include "ruby.h"
#include <magic.h>
#include <math.h>

#define GetMagicCookie(obj, cookie) {\
  if (RTEST(rb_magic_closed_p(obj)))\
    rb_raise(rb_eRuntimeError, "closed stream");\
  Data_Get_Struct(obj, struct magic_set, cookie);\
}

static VALUE cFileMagic, rb_FileMagicError;

static VALUE rb_magic_new(int, VALUE*, VALUE);
static VALUE rb_magic_init(VALUE, VALUE);

static VALUE rb_magic_close(VALUE);
static VALUE rb_magic_closed_p(VALUE);
static VALUE rb_magic_file(int, VALUE*, VALUE);
static VALUE rb_magic_buffer(int, VALUE*, VALUE);
static VALUE rb_magic_getflags(VALUE);
static VALUE rb_magic_setflags(VALUE, VALUE);
static VALUE rb_magic_check(int, VALUE*, VALUE);
static VALUE rb_magic_compile(VALUE, VALUE);

static VALUE rb_magic_flags_to_int(int, VALUE*);
static VALUE rb_magic_apply_simple(VALUE, const char*, int, VALUE*);

static void rb_magic_free(magic_t);
void Init_filemagic(void);

#endif /* _FILEMAGIC_H */
