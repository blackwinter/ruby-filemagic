/*********************************************************

 filemagic.c

 a Ruby extension to bind Ruby to the libmagic library
 (ftp://ftp.astron.com/pub/file/)

 patterned very closely off of audiofile.c by jaredj which
 is patterned very very closely off of gdbm.c by matz ;-)

 $Author: twhitton $
 $Date: 2003/07/30 13:25:10 $

*********************************************************/

#include "filemagic.h"

/* FileMagic.new */
static VALUE
rb_magic_new(int argc, VALUE *argv, VALUE class) {
  VALUE obj, args[2];
  magic_t cookie;

  if (rb_block_given_p()) {
    rb_warn("FileMagic::new() does not take block; use FileMagic::open() instead");
  }

  if (argc > 0 && TYPE(args[1] = argv[argc - 1]) == T_HASH) {
    argc--;
  }
  else {
    args[1] = rb_hash_new();
  }

  args[0] = rb_magic_flags_to_int(argc, argv);
  if ((cookie = magic_open(NUM2INT(args[0]))) == NULL) {
    rb_fatal("out of memory");
  }

  if (magic_load(cookie, NULL) == -1) {
    rb_fatal("%s", magic_error(cookie));
  }

  obj = Data_Wrap_Struct(class, 0, rb_magic_free, cookie);
  rb_obj_call_init(obj, 2, args);

  return obj;
}

static VALUE
rb_magic_init(int argc, VALUE *argv, VALUE self) {
  VALUE flags, options, keys, k, m;
  ID mid;
  int i;

  if (rb_scan_args(argc, argv, "11", &flags, &options) == 1) {
    options = rb_hash_new();
  }

  rb_iv_set(self, "_flags", flags);
  rb_iv_set(self, "closed", Qfalse);

  keys = rb_funcall(options, rb_intern("keys"), 0);

  for (i = 0; i < RARRAY_LEN(keys); i++) {
    k = rb_funcall(keys, rb_intern("[]"), 1, INT2FIX(i));
    m = rb_funcall(rb_funcall(k, rb_intern("to_s"), 0),
      rb_intern("+"), 1, rb_str_new2("=")
    );

    if (rb_respond_to(self, mid = rb_intern(StringValueCStr(m)))) {
      rb_funcall(self, mid, 1, rb_hash_aref(options, k));
    }
    else {
      k = rb_funcall(k, rb_intern("inspect"), 0);
      rb_raise(rb_eArgError, "illegal option: %s", StringValueCStr(k));
    }
  }

  return Qnil;
}

/* Frees resources allocated */
static VALUE
rb_magic_close(VALUE self) {
  magic_t cookie;

  if (RTEST(rb_magic_closed_p(self))) {
    return Qnil;
  }

  GetMagicCookie(self, cookie);
  magic_close(cookie);

  /* This keeps rb_magic_free from trying to free closed objects */
  RDATA(self)->data = NULL;

  rb_iv_set(self, "closed", Qtrue);

  return Qnil;
}

static VALUE
rb_magic_closed_p(VALUE self) {
  return rb_attr_get(self, rb_intern("closed"));
}

/* Return a string describing file */
static VALUE
rb_magic_file(int argc, VALUE *argv, VALUE self) {
  VALUE file, s;
  const char *m;
  magic_t cookie;

  rb_scan_args(argc, argv, "11", &file, &s);

  m = StringValuePtr(file);
  GetMagicCookie(self, cookie);

  if ((m = magic_file(cookie, m)) == NULL) {
    rb_raise(rb_FileMagicError, "%s", magic_error(cookie));
  }

  return rb_magic_apply_simple(self, m, s);
}

/* Return a string describing the string buffer */
static VALUE
rb_magic_buffer(int argc, VALUE *argv, VALUE self) {
  VALUE buffer, s;
  const char *m;
  magic_t cookie;

  rb_scan_args(argc, argv, "11", &buffer, &s);

  m = StringValuePtr(buffer);
  GetMagicCookie(self, cookie);

  if ((m = magic_buffer(cookie, m, RSTRING_LEN(buffer))) == NULL) {
    rb_raise(rb_FileMagicError, "%s", magic_error(cookie));
  }

  return rb_magic_apply_simple(self, m, s);
}

/* Get the flags as array of symbols */
static VALUE
rb_magic_getflags(VALUE self) {
  VALUE ary = rb_ary_new();
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_INT"));
  int i = NUM2INT(rb_attr_get(self, rb_intern("_flags"))), j = 0;

  while ((i -= j) > 0) {
    j = pow(2, (int)(log(i) / log(2)));
    rb_ary_unshift(ary, rb_hash_aref(map, INT2FIX(j)));
  }

  return ary;
}

/* Set flags on the cookie object */
static VALUE
rb_magic_setflags(VALUE self, VALUE flags) {
  int retval;
  magic_t cookie;

  flags = rb_Array(flags);
  flags = rb_magic_flags_to_int(RARRAY_LEN(flags), RARRAY_PTR(flags));
  rb_iv_set(self, "_flags", flags);

  GetMagicCookie(self, cookie);
  retval = magic_setflags(cookie, NUM2INT(flags));

  return INT2FIX(retval);
}

/* Checks validity of a magic database file */
static VALUE
rb_magic_check(int argc, VALUE *argv, VALUE self) {
  VALUE s;
  const char *file;
  int retval;
  magic_t cookie;

  file = rb_scan_args(argc, argv, "01", &s) == 1 ? StringValuePtr(s) : NULL;

  GetMagicCookie(self, cookie);
  retval = magic_check(cookie, file);

  return INT2FIX(retval);
}

/* Compiles a magic database file */
static VALUE
rb_magic_compile(VALUE self, VALUE file) {
  int retval;
  const char *m;
  magic_t cookie;

  GetMagicCookie(self, cookie);
  m = StringValuePtr(file);
  retval = magic_compile(cookie, m);

  return INT2FIX(retval);
}

static VALUE
rb_magic_flags_to_int(int argc, VALUE *argv) {
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_SYM"));
  VALUE f, g;
  int i = MAGIC_NONE, j;

  for (j = 0; j < argc; j++) {
    f = argv[j];

    switch (TYPE(f)) {
      case T_SYMBOL:
        if (RTEST(g = rb_hash_aref(map, f))) {
          f = g;
          /* fall through */
        }
        else {
          f = rb_funcall(f, rb_intern("inspect"), 0);
          rb_raise(rb_eArgError,
            "%s: %s", NIL_P(g) ? "no such flag" : "flag not available",
            StringValueCStr(f)
          );
          break;
        }
      case T_FIXNUM:
        i |= NUM2INT(f);
        break;
      default:
        rb_raise(rb_eTypeError,
          "wrong argument type %s (expected Fixnum or Symbol)",
          rb_obj_classname(f)
        );
    }
  }

  return INT2FIX(i);
}

static VALUE
rb_magic_apply_simple(VALUE self, const char *m, VALUE s) {
  VALUE str = rb_str_new2(m);

  if (RTEST(NIL_P(s) ? rb_attr_get(self, rb_intern("@simplified")) : s)) {
    rb_funcall(str, rb_intern("downcase!"), 0);

    return rb_funcall(str, rb_intern("slice"), 2,
      rb_const_get(cFileMagic, rb_intern("SIMPLE_RE")),
      INT2FIX(1)
    );
  }
  else {
    return str;
  }
}

/*
 GC never seems to happen until the program terminates, but this is called
 on any unclosed objects
*/
static void
rb_magic_free(magic_t cookie) {
  magic_close(cookie);
}

void
Init_filemagic() {
  cFileMagic = rb_define_class("FileMagic", rb_cObject);

  rb_define_singleton_method(cFileMagic, "new", rb_magic_new, -1);

  rb_define_method(cFileMagic, "initialize", rb_magic_init,     -1);
  rb_define_method(cFileMagic, "close",      rb_magic_close,     0);
  rb_define_method(cFileMagic, "closed?",    rb_magic_closed_p,  0);
  rb_define_method(cFileMagic, "file",       rb_magic_file,     -1);
  rb_define_method(cFileMagic, "buffer",     rb_magic_buffer,   -1);
  rb_define_method(cFileMagic, "flags",      rb_magic_getflags,  0);
  rb_define_method(cFileMagic, "flags=",     rb_magic_setflags,  1);
  rb_define_method(cFileMagic, "check",      rb_magic_check,    -1);
  rb_define_method(cFileMagic, "compile",    rb_magic_compile,   1);

  rb_alias(cFileMagic, rb_intern("valid?"), rb_intern("check"));

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
#ifdef MAGIC_APPLE
  rb_define_const(cFileMagic, "MAGIC_APPLE",             INT2FIX(MAGIC_APPLE));
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
#ifdef MAGIC_NO_CHECK_TEXT
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TEXT",     INT2FIX(MAGIC_NO_CHECK_TEXT));
#endif
#ifdef MAGIC_NO_CHECK_CDF
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_CDF",      INT2FIX(MAGIC_NO_CHECK_CDF));
#endif
#ifdef MAGIC_NO_CHECK_TOKENS
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TOKENS",   INT2FIX(MAGIC_NO_CHECK_TOKENS));
#endif
#ifdef MAGIC_NO_CHECK_ENCODING
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ENCODING", INT2FIX(MAGIC_NO_CHECK_ENCODING));
#endif
#ifdef MAGIC_NO_CHECK_ASCII
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_ASCII",    INT2FIX(MAGIC_NO_CHECK_ASCII));
#endif
#ifdef MAGIC_NO_CHECK_FORTRAN
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_FORTRAN",  INT2FIX(MAGIC_NO_CHECK_FORTRAN));
#endif
#ifdef MAGIC_NO_CHECK_TROFF
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_TROFF",    INT2FIX(MAGIC_NO_CHECK_TROFF));
#endif
}
