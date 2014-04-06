#ifndef FILEMAGIC_H
#define FILEMAGIC_H

#include "ruby.h"
#include <math.h>
#include <magic.h>
#ifdef HAVE_FILE_PATCHLEVEL_H
#include <file/patchlevel.h>
#endif

/* Ruby 1.8.5 compatibility */
#ifndef RSTRING_LEN
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif
#ifndef RARRAY_LEN
#define RARRAY_LEN(s) (RARRAY(s)->len)
#endif

#define GetMagicCookie(obj, cookie) {\
  if (RTEST(rb_magic_closed_p(obj))) {\
    rb_raise(rb_eRuntimeError, "closed stream");\
  }\
  else {\
    Data_Get_Struct((obj), struct magic_set, (cookie));\
  }\
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

static VALUE rb_magic_flags_to_int(VALUE);
static VALUE rb_magic_apply_simple(VALUE, const char*, VALUE);

static void rb_magic_free(magic_t);
void Init_ruby_filemagic(void);

#endif /* FILEMAGIC_H */
