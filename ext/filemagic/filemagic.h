#ifndef FILEMAGIC_H
#define FILEMAGIC_H

#include "ruby.h"
#include <math.h>
#include <errno.h>
#include <magic.h>
#ifdef HAVE_FILE_PATCHLEVEL_H
#include <file/patchlevel.h>
#endif

#define GetMagicSet(obj, ms) {\
  if (RTEST(rb_magic_closed_p(obj))) {\
    rb_raise(rb_eRuntimeError, "closed stream");\
  }\
  else {\
    Data_Get_Struct((obj), struct magic_set, (ms));\
  }\
}

#define RB_MAGIC_TYPE_FILE       magic_file(ms, StringValuePtr(arg))
#define RB_MAGIC_TYPE_BUFFER     magic_buffer(ms, StringValuePtr(arg), RSTRING_LEN(arg))
#define RB_MAGIC_TYPE_DESCRIPTOR magic_descriptor(ms, NUM2INT(arg))

#define RB_MAGIC_TYPE(what, WHAT) \
static VALUE \
rb_magic_##what(int argc, VALUE *argv, VALUE self) {\
  VALUE arg, simple, res;\
  const char *type;\
  magic_t ms;\
\
  rb_scan_args(argc, argv, "11", &arg, &simple);\
  GetMagicSet(self, ms);\
\
  if ((type = RB_MAGIC_TYPE_##WHAT) == NULL) {\
    rb_raise(rb_FileMagicError, "failed lookup: %s", magic_error(ms));\
  }\
\
  res = rb_str_new2(type);\
\
  if (NIL_P(simple)) {\
    simple = rb_attr_get(self, rb_intern("@simplified"));\
  }\
\
  if (RTEST(simple)) {\
    rb_funcall(res, rb_intern("downcase!"), 0);\
\
    return rb_funcall(res, rb_intern("slice"), 2,\
      rb_const_get(cFileMagic, rb_intern("SIMPLE_RE")), INT2FIX(1));\
  }\
  else {\
    return res;\
  }\
}

#define RB_MAGIC_APPRENTICE(what) \
static VALUE \
rb_magic_##what(int argc, VALUE *argv, VALUE self) {\
  VALUE str;\
  const char *file;\
  magic_t ms;\
\
  file = rb_scan_args(argc, argv, "01", &str) == 1 ? StringValuePtr(str) : NULL;\
\
  GetMagicSet(self, ms);\
\
  return magic_##what(ms, file) ? Qfalse : Qtrue;\
}

#define RB_MAGIC_SET_VERSION(m, p) sprintf(version, "%d.%02d", m, p);

static VALUE cFileMagic, rb_FileMagicError;

static VALUE rb_magic_version(VALUE);
static VALUE rb_magic_getpath(VALUE);
static VALUE rb_magic_flags(VALUE, VALUE);

static VALUE rb_magic_new(int, VALUE*, VALUE);
static void rb_magic_free(magic_t);
static VALUE rb_magic_init(int, VALUE*, VALUE);

static VALUE rb_magic_close(VALUE);
static VALUE rb_magic_closed_p(VALUE);

static VALUE rb_magic_file(int, VALUE*, VALUE);
static VALUE rb_magic_buffer(int, VALUE*, VALUE);
static VALUE rb_magic_descriptor(int, VALUE*, VALUE);

static VALUE rb_magic_getflags(VALUE);
static VALUE rb_magic_setflags(VALUE, VALUE);

static VALUE rb_magic_list(int, VALUE*, VALUE);
static VALUE rb_magic_load(int, VALUE*, VALUE);
static VALUE rb_magic_check(int, VALUE*, VALUE);
static VALUE rb_magic_compile(int, VALUE*, VALUE);

void Init_ruby_filemagic(void);

#endif /* FILEMAGIC_H */
