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
    Data_Get_Struct(obj, struct magic_set, cookie);\
}

/* 
   GC never seems to happen until the program terminates, but this is called
   on any unclosed objects
*/
static void magick_free(magic_t cookie)
{
    magic_close(cookie);
}

/* This is mainly for subclassing */
static VALUE magick_init(VALUE self, VALUE flags)
{
    /* This isn't used internally, but might as well store it for children */
    rb_iv_set(self, "@flags", flags);
    return self;
}

/* Frees resources allocated */
static VALUE magick_close(VALUE class)
{
    magic_t cookie;
    GetMagicCookie(class, cookie);
    magic_close(cookie);
    /* This keeps magick_free from trying to free closed objects */
    RDATA(class)->data = NULL;
    return Qnil;
}

/* FileMagic.new */
static VALUE magick_new(VALUE class, VALUE flags)
{
    VALUE obj;
    magic_t cookie;
    cookie = magic_open(NUM2INT(flags));
    if (cookie == NULL) {
        rb_fatal("out of memory");
    }
    if (magic_load(cookie, NULL) == -1) {
        rb_fatal("%s", magic_error(cookie));
    }
    rb_FileMagicError = rb_define_class("FileMagicError", rb_eStandardError);
    obj = Data_Wrap_Struct(class, 0, magick_free, cookie);
    rb_obj_call_init(obj, 1, &flags);
    return obj;
}

/* Return a string describing file */
static VALUE magick_file(VALUE class, VALUE file)
{
    const char *m;
    magic_t cookie;
    m = STR2CSTR(file);
    GetMagicCookie(class, cookie);
    if ((m = magic_file(cookie, m)) == NULL)
        rb_raise(rb_FileMagicError, magic_error(cookie));
    return rb_str_new2(m);
}

/* Return a string describing the string buffer */
static VALUE magick_buffer(VALUE class, VALUE buffer)
{
    int i = RSTRING(buffer)->len;
    const char *m;
    magic_t cookie;
    m = STR2CSTR(buffer);
    GetMagicCookie(class, cookie);
    if ((m = magic_buffer(cookie, m, i)) == NULL)
        rb_raise(rb_FileMagicError, magic_error(cookie));
    return rb_str_new2(m);
}

/* Checks validity of a magic database file */
static VALUE magick_check(VALUE class, VALUE file)
{
    int retval;
    const char *m;
    magic_t cookie;
    GetMagicCookie(class, cookie);
    m = STR2CSTR(file);
    retval = magic_check(cookie, m);
    return INT2FIX(retval);
}

/* Compiles a magic database file */
static VALUE magick_compile(VALUE class, VALUE file)
{
    int retval;
    const char *m;
    magic_t cookie;
    GetMagicCookie(class, cookie);
    m = STR2CSTR(file);
    retval = magic_compile(cookie, m);
    return INT2FIX(retval);
}

void Init_filemagic() {
    cFileMagic = rb_define_class("FileMagic", rb_cObject);
    rb_define_singleton_method(cFileMagic, "new", magick_new, 1);
    rb_define_method(cFileMagic, "initialize", magick_init, 1);
    rb_define_method(cFileMagic, "close", magick_close, 0);
    rb_define_method(cFileMagic, "file", magick_file, 1);
    rb_define_method(cFileMagic, "buffer", magick_buffer, 1);
    rb_define_method(cFileMagic, "check", magick_check, 1);
    rb_define_method(cFileMagic, "compile", magick_compile, 1);
    rb_define_const(cFileMagic, "MAGIC_NONE", INT2FIX(MAGIC_NONE));
    rb_define_const(cFileMagic, "MAGIC_DEBUG", INT2FIX(MAGIC_DEBUG));
    rb_define_const(cFileMagic, "MAGIC_SYMLINK", INT2FIX(MAGIC_SYMLINK));
    rb_define_const(cFileMagic, "MAGIC_COMPRESS", INT2FIX(MAGIC_COMPRESS));
    rb_define_const(cFileMagic, "MAGIC_DEVICES", INT2FIX(MAGIC_DEVICES));
    rb_define_const(cFileMagic, "MAGIC_MIME", INT2FIX(MAGIC_MIME));
    rb_define_const(cFileMagic, "MAGIC_CONTINUE", INT2FIX(MAGIC_CONTINUE));
    rb_define_const(cFileMagic, "MAGIC_CHECK", INT2FIX(MAGIC_CHECK));
}
