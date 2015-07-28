#include "filemagic.h"

/* Returns the magic version */
static VALUE
rb_magic_version(VALUE klass) {
  char version[8] = "0";
#ifdef HAVE_MAGIC_VERSION
  RB_MAGIC_SET_VERSION(magic_version() / 100, magic_version() % 100)
#endif
  return rb_str_new2(version);
}

/* Returns the magic path */
static VALUE
rb_magic_getpath(VALUE klass) {
  const char *path = magic_getpath(NULL, 0);
  return path != NULL ? rb_str_new2(path) : Qnil;
}

/* Converts flags to integer */
static VALUE
rb_magic_flags(VALUE klass, VALUE flags) {
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_SYM")), f, g;
  int i = MAGIC_NONE, j;

  if (TYPE(flags) != T_ARRAY) {
    rb_raise(rb_eTypeError,
      "wrong argument type %s (expected Array)",
      rb_obj_classname(flags));
  }

  for (j = 0; j < RARRAY_LEN(flags); j++) {
    f = rb_ary_entry(flags, j);

    switch (TYPE(f)) {
      case T_SYMBOL:
        if (RTEST(g = rb_hash_aref(map, f))) {
          f = g;
          /* fall through */
        }
        else {
          f = rb_funcall(f, rb_intern("inspect"), 0);
          rb_raise(rb_eArgError, "%s: %s",
            NIL_P(g) ? "no such flag" : "flag not available",
            StringValueCStr(f));

          break;
        }
      case T_FIXNUM:
        i |= NUM2INT(f);
        break;
      default:
        rb_raise(rb_eTypeError,
          "wrong argument type %s (expected Fixnum or Symbol)",
          rb_obj_classname(f));
    }
  }

  return INT2FIX(i);
}

/* FileMagic.new */
static VALUE
rb_magic_new(int argc, VALUE *argv, VALUE klass) {
  VALUE obj, args[2];
  magic_t ms;

  if (rb_block_given_p()) {
    rb_warn(
      "FileMagic.new() does not take a block; use FileMagic.open() instead");
  }

  if (argc > 0 && TYPE(args[1] = argv[argc - 1]) == T_HASH) {
    argc--;
  }
  else {
    args[1] = rb_hash_new();
  }

  args[0] = rb_magic_flags(klass, rb_ary_new4(argc, argv));

  if ((ms = magic_open(NUM2INT(args[0]))) == NULL) {
    rb_raise(rb_FileMagicError,
      "failed to initialize magic cookie (%d)", errno || -1);
  }

  if (magic_load(ms, NULL) == -1) {
    rb_raise(rb_FileMagicError,
      "failed to load database: %s", magic_error(ms));
  }

  obj = Data_Wrap_Struct(klass, 0, rb_magic_free, ms);
  rb_obj_call_init(obj, 2, args);

  return obj;
}

static void
rb_magic_free(magic_t ms) {
  magic_close(ms);
}

static VALUE
rb_magic_init(int argc, VALUE *argv, VALUE self) {
  VALUE flags, options, keys, k, m;
  ID mid;
  int i;

  if (rb_scan_args(argc, argv, "11", &flags, &options) == 1) {
    options = rb_hash_new();
  }

  rb_iv_set(self, "iflags", flags);
  rb_iv_set(self, "closed", Qfalse);

  keys = rb_funcall(options, rb_intern("keys"), 0);

  for (i = 0; i < RARRAY_LEN(keys); i++) {
    k = rb_ary_entry(keys, i);
    m = rb_str_plus(rb_obj_as_string(k), rb_str_new2("="));

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
  magic_t ms;

  if (RTEST(rb_magic_closed_p(self))) {
    return Qnil;
  }

  GetMagicSet(self, ms);
  rb_magic_free(ms);

  /* This keeps rb_magic_free from trying to free closed objects */
  DATA_PTR(self) = NULL;

  rb_iv_set(self, "closed", Qtrue);

  return Qnil;
}

static VALUE
rb_magic_closed_p(VALUE self) {
  return rb_attr_get(self, rb_intern("closed"));
}

/* Return a string describing the named file */
RB_MAGIC_TYPE(file, FILE)

/* Return a string describing the string buffer */
RB_MAGIC_TYPE(buffer, BUFFER)

/* Return a string describing the file descriptor */
RB_MAGIC_TYPE(descriptor, DESCRIPTOR)

/* Get the flags as array of symbols */
static VALUE
rb_magic_getflags(VALUE self) {
  VALUE ary = rb_ary_new();
  VALUE map = rb_const_get(cFileMagic, rb_intern("FLAGS_BY_INT"));
  int i = NUM2INT(rb_attr_get(self, rb_intern("iflags"))), j = 0;

  while ((i -= j) > 0) {
    j = pow(2, (int)(log(i) / log(2)));
    rb_ary_unshift(ary, rb_hash_aref(map, INT2FIX(j)));
  }

  return ary;
}

/* Set flags on the ms object */
static VALUE
rb_magic_setflags(VALUE self, VALUE flags) {
  magic_t ms;

  GetMagicSet(self, ms);

  rb_iv_set(self, "iflags",
    flags = rb_magic_flags(CLASS_OF(self), rb_Array(flags)));

  return INT2FIX(magic_setflags(ms, NUM2INT(flags)));
}

/* Lists a magic database file */
RB_MAGIC_APPRENTICE(list)

/* Loads a magic database file */
RB_MAGIC_APPRENTICE(load)

/* Checks validity of a magic database file */
RB_MAGIC_APPRENTICE(check)

/* Compiles a magic database file */
RB_MAGIC_APPRENTICE(compile)

void
Init_ruby_filemagic() {
  char version[8] = "0";
  cFileMagic = rb_define_class("FileMagic", rb_cObject);

#if defined(FILE_VERSION_MAJOR)
  RB_MAGIC_SET_VERSION(FILE_VERSION_MAJOR, patchlevel)
#elif defined(MAGIC_VERSION)
  RB_MAGIC_SET_VERSION(MAGIC_VERSION / 100, MAGIC_VERSION % 100)
#endif

  rb_define_const(cFileMagic, "MAGIC_VERSION", rb_str_new2(version));

  rb_define_singleton_method(cFileMagic, "library_version", rb_magic_version,  0);
  rb_define_singleton_method(cFileMagic, "path",            rb_magic_getpath,  0);
  rb_define_singleton_method(cFileMagic, "flags",           rb_magic_flags,    1);
  rb_define_singleton_method(cFileMagic, "new",             rb_magic_new,     -1);

  rb_define_method(cFileMagic, "initialize", rb_magic_init,       -1);
  rb_define_method(cFileMagic, "close",      rb_magic_close,       0);
  rb_define_method(cFileMagic, "closed?",    rb_magic_closed_p,    0);
  rb_define_method(cFileMagic, "file",       rb_magic_file,       -1);
  rb_define_method(cFileMagic, "buffer",     rb_magic_buffer,     -1);
  rb_define_method(cFileMagic, "descriptor", rb_magic_descriptor, -1);
  rb_define_method(cFileMagic, "flags",      rb_magic_getflags,    0);
  rb_define_method(cFileMagic, "flags=",     rb_magic_setflags,    1);
  rb_define_method(cFileMagic, "list",       rb_magic_list,       -1);
  rb_define_method(cFileMagic, "load",       rb_magic_load,       -1);
  rb_define_method(cFileMagic, "check",      rb_magic_check,      -1);
  rb_define_method(cFileMagic, "compile",    rb_magic_compile,    -1);

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
#ifdef MAGIC_EXTENSION
  rb_define_const(cFileMagic, "MAGIC_EXTENSION",         INT2FIX(MAGIC_EXTENSION));
#endif
#ifdef MAGIC_COMPRESS_TRANSP
  rb_define_const(cFileMagic, "MAGIC_COMPRESS_TRANSP",   INT2FIX(MAGIC_COMPRESS_TRANSP));
#endif
#ifdef MAGIC_NODESC
  rb_define_const(cFileMagic, "MAGIC_NODESC",            INT2FIX(MAGIC_NODESC));
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
#if defined(MAGIC_NO_CHECK_BUILTIN) && MAGIC_VERSION > 514
  /* defined in b5be901 (2010-01-28, 5.05), but broken until 38e0136 (2013-08-15, 5.15) */
  rb_define_const(cFileMagic, "MAGIC_NO_CHECK_BUILTIN",  INT2FIX(MAGIC_NO_CHECK_BUILTIN));
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
