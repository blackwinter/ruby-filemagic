#ifndef __FILEMAGIC_H_
#define __FILEMAGIC_H_

#include "ruby.h"
#include <magic.h>
#include <math.h>

/* Ruby 1.8.5 compatibility */
#ifndef RSTRING_LEN
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif
#ifndef RSTRING_PTR
#define RSTRING_PTR(s) (RSTRING(s)->ptr)
#endif
#ifndef RARRAY_LEN
#define RARRAY_LEN(s) (RARRAY(s)->len)
#endif
#ifndef RARRAY_PTR
#define RARRAY_PTR(s) (RARRAY(s)->ptr)
#endif

#define GetMagicCookie(obj, cookie) {\
  if (RTEST(rb_magic_closed_p(obj)))\
    rb_raise(rb_eRuntimeError, "closed stream");\
  Data_Get_Struct(obj, struct magic_set, cookie);\
}

static VALUE cFileMagic, rb_FileMagicError;

static VALUE rb_magic_new(int, VALUE*, VALUE);
static VALUE rb_magic_init(int, VALUE*, VALUE);

static VALUE rb_magic_close(VALUE);
static VALUE rb_magic_closed_p(VALUE);
static VALUE rb_magic_file(int, VALUE*, VALUE);
static VALUE rb_magic_buffer(int, VALUE*, VALUE);
static VALUE rb_magic_getflags(VALUE);
static VALUE rb_magic_setflags(VALUE, VALUE);
static VALUE rb_magic_check(int, VALUE*, VALUE);
static VALUE rb_magic_compile(VALUE, VALUE);

static VALUE rb_magic_flags_to_int(int, VALUE*);
static VALUE rb_magic_apply_simple(VALUE, const char*, VALUE);

static void rb_magic_free(magic_t);
void Init_filemagic(void);

#endif /* __FILEMAGIC_H_ */
