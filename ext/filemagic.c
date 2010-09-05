/*********************************************************

 filemagic.c

 a Ruby extension to bind Ruby to the libmagic library
 (ftp://ftp.astron.com/pub/file/)

 patterned very closely off of audiofile.c by jaredj which
 is patterned very very closely off of gdbm.c by matz ;-)

 $Author: twhitton $
 $Date: 2003/07/30 13:25:10 $

*********************************************************/

#include "ruby.h"
#include <magic.h>

static VALUE cFileMagic, rb_FileMagicError;

#define GetMagicCookie(obj, cookie) {\
  if (rb_funcall(obj, rb_intern("closed?"), 0))\
    rb_raise(rb_eRuntimeError, "closed stream");\
  Data_Get_Struct(obj, struct magic_set, cookie);\
}

/* 
 GC never seems to happen until the program terminates, but this is called
 on any unclosed objects
*/
static void rb_magic_free(magic_t cookie) {
  magic_close(cookie);
}

static VALUE rb_magic_init(VALUE self, VALUE flags) {
}

/* Frees resources allocated */
static VALUE rb_magic_close(VALUE self) {
  magic_t cookie;

  GetMagicCookie(self, cookie);
  magic_close(cookie);

  /* This keeps rb_magic_free from trying to free closed objects */
  RDATA(self)->data = NULL;

  return Qnil;
}

/* FileMagic.new */
static VALUE rb_magic_new(VALUE class, VALUE flags) {
  VALUE obj;
  magic_t cookie;

  cookie = magic_open(NUM2INT(flags));
  if (cookie == NULL)
    rb_fatal("out of memory");
  if (magic_load(cookie, NULL) == -1)
    rb_fatal("%s", magic_error(cookie));

  obj = Data_Wrap_Struct(class, 0, rb_magic_free, cookie);
  rb_obj_call_init(obj, 1, &flags);

  return obj;
}

/* Return a string describing file */
static VALUE rb_magic_file(VALUE self, VALUE file) {
  const char *m;
  magic_t cookie;

  m = StringValuePtr(file);
  GetMagicCookie(self, cookie);
  if ((m = magic_file(cookie, m)) == NULL)
    rb_raise(rb_FileMagicError, magic_error(cookie));

  return rb_str_new2(m);
}

/* Return a string describing the string buffer */
static VALUE rb_magic_buffer(VALUE self, VALUE buffer) {
  int i = RSTRING(buffer)->len;
  const char *m;
  magic_t cookie;

  m = StringValuePtr(buffer);
  GetMagicCookie(self, cookie);
  if ((m = magic_buffer(cookie, m, i)) == NULL)
    rb_raise(rb_FileMagicError, magic_error(cookie));

  return rb_str_new2(m);
}

/* Set flags on the cookie object */
static VALUE rb_magic_setflags(VALUE self, VALUE flags) {
  int retval;
  magic_t cookie;

  GetMagicCookie(self, cookie);
  retval = magic_setflags(cookie, NUM2INT(flags));

  return INT2FIX(retval);
}

/* Checks validity of a magic database file */
static VALUE rb_magic_check(VALUE self, VALUE file) {
  int retval;
  const char *m;
  magic_t cookie;

  GetMagicCookie(self, cookie);
  m = StringValuePtr(file);
  retval = magic_check(cookie, m);

  return INT2FIX(retval);
}

/* Compiles a magic database file */
static VALUE rb_magic_compile(VALUE self, VALUE file) {
  int retval;
  const char *m;
  magic_t cookie;

  GetMagicCookie(self, cookie);
  m = StringValuePtr(file);
  retval = magic_compile(cookie, m);

  return INT2FIX(retval);
}

void Init_filemagic() {
  cFileMagic = rb_define_class("FileMagic", rb_cObject);

  rb_define_singleton_method(cFileMagic, "fm_new", rb_magic_new, 1);

  rb_define_method(cFileMagic, "fm_initialize", rb_magic_init,     1);
  rb_define_method(cFileMagic, "fm_close",      rb_magic_close,    0);
  rb_define_method(cFileMagic, "fm_file",       rb_magic_file,     1);
  rb_define_method(cFileMagic, "fm_buffer",     rb_magic_buffer,   1);
  rb_define_method(cFileMagic, "fm_setflags",   rb_magic_setflags, 1);
  rb_define_method(cFileMagic, "fm_check",      rb_magic_check,    1);
  rb_define_method(cFileMagic, "fm_compile",    rb_magic_compile,  1);

  rb_FileMagicError = rb_define_class_under(cFileMagic, "FileMagicError", rb_eStandardError);

  #ifdef MAGIC_NONE
  rb_define_const(cFileMagic, "MAGIC_NONE",              INT2FIX(MAGIC_NONE));
  #endif
  #ifdef MAGIC_DEBUG
  rb_define_const(cFileMagic, "MAGIC_DEBUG",             INT2FIX(MAGIC_DEBUG));
  #endif
  #ifdef MAGIC_SYMLINK
  rb_define_const(cFileMagic, "MAGIC_SYMLINK",           INT2FIX(MAGIC_SYMLINK));
  #endif
  #ifdef MAGIC_COMPRESS
  rb_define_const(cFileMagic, "MAGIC_COMPRESS",          INT2FIX(MAGIC_COMPRESS));
  #endif
  #ifdef MAGIC_DEVICES
  rb_define_const(cFileMagic, "MAGIC_DEVICES",           INT2FIX(MAGIC_DEVICES));
  #endif
  #ifdef MAGIC_MIME_TYPE
  rb_define_const(cFileMagic, "MAGIC_MIME_TYPE",         INT2FIX(MAGIC_MIME_TYPE));
  #endif
  #ifdef MAGIC_CONTINUE
  rb_define_const(cFileMagic, "MAGIC_CONTINUE",          INT2FIX(MAGIC_CONTINUE));
  #endif
  #ifdef MAGIC_CHECK
  rb_define_const(cFileMagic, "MAGIC_CHECK",             INT2FIX(MAGIC_CHECK));
  #endif
  #ifdef MAGIC_PRESERVE_ATIME
  rb_define_const(cFileMagic, "MAGIC_PRESERVE_ATIME",    INT2FIX(MAGIC_PRESERVE_ATIME));
  #endif
  #ifdef MAGIC_RAW
  rb_define_const(cFileMagic, "MAGIC_RAW",               INT2FIX(MAGIC_RAW));
  #endif
  #ifdef MAGIC_ERROR
  rb_define_const(cFileMagic, "MAGIC_ERROR",             INT2FIX(MAGIC_ERROR));
  #endif
  #ifdef MAGIC_MIME_ENCODING
  rb_define_const(cFileMagic, "MAGIC_MIME_ENCODING",     INT2FIX(MAGIC_MIME_ENCODING));
  #endif
  #ifdef MAGIC_MIME
  rb_define_const(cFileMagic, "MAGIC_MIME",              INT2FIX(MAGIC_MIME));
  #endif
  #ifdef MAGIC_NO_CHECK_COMPRESS
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_COMPRESS", INT2FIX(MAGIC_NO_CHECK_COMPRESS));
  #endif
  #ifdef MAGIC_NO_CHECK_TAR
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TAR",      INT2FIX(MAGIC_NO_CHECK_TAR));
  #endif
  #ifdef MAGIC_NO_CHECK_SOFT
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_SOFT",     INT2FIX(MAGIC_NO_CHECK_SOFT));
  #endif
  #ifdef MAGIC_NO_CHECK_APPTYPE
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_APPTYPE",  INT2FIX(MAGIC_NO_CHECK_APPTYPE));
  #endif
  #ifdef MAGIC_NO_CHECK_ELF
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ELF",      INT2FIX(MAGIC_NO_CHECK_ELF));
  #endif
  #ifdef MAGIC_NO_CHECK_ASCII
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ASCII",    INT2FIX(MAGIC_NO_CHECK_ASCII));
  #endif
  #ifdef MAGIC_NO_CHECK_TROFF
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TROFF",    INT2FIX(MAGIC_NO_CHECK_TROFF));
  #endif
  #ifdef MAGIC_NO_CHECK_TOKENS
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TOKENS",   INT2FIX(MAGIC_NO_CHECK_TOKENS));
  #endif
  #ifdef MAGIC_NO_CHECK_FORTRAN
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_FORTRAN",  INT2FIX(MAGIC_NO_CHECK_FORTRAN));
  #endif
}
