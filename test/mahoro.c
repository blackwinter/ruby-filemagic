/*
 * This file is Public Domain.
 */

#include <ruby.h>
#include <magic.h>

struct MagicCookie
{
	magic_t cookie;
};

static VALUE cMahoro;
static VALUE eMahoroError;

static void
mahoro_free(ptr)
	struct MagicCookie *ptr;
{
	magic_close(ptr->cookie);
	free(ptr);
}

static VALUE
mahoro_allocate(klass)
	VALUE klass;
{
	return Data_Wrap_Struct(klass, 0, mahoro_free, 0);
}

static VALUE
mahoro_initialize(argc, argv, self)
	int argc;
	VALUE *argv, self;
{
	int flags = MAGIC_NONE;
	char *path = 0;
	struct MagicCookie *ptr;
	magic_t cookie;
	VALUE vpath, vflags;

	switch(rb_scan_args(argc, argv, "02", &vflags, &vpath)) {
		case 2:
			if(!NIL_P(vpath)) {
				path = StringValuePtr(vpath);
			}
			/* fallthrough */
		case 1:
			flags = FIX2INT(vflags);
			break;
	}

	if(!(cookie = magic_open(flags))) {
		rb_raise(eMahoroError, "failed to initialize magic cookie");
	}

	if(magic_load(cookie, path)) {
		rb_raise(eMahoroError, "failed to load database: %s",
		                      magic_error(cookie));
	}

	ptr = ALLOC(struct MagicCookie);
	ptr->cookie = cookie;
	DATA_PTR(self) = ptr;

	return self;
}

static VALUE
mahoro_file(self, path)
	VALUE self, path;
{
	const char *msg;
	magic_t cookie = ((struct MagicCookie *) DATA_PTR(self))->cookie;

	if(!(msg = magic_file(cookie, StringValuePtr(path)))) {
		rb_raise(eMahoroError, "failed lookup: %s", magic_error(cookie));
	}

	return rb_str_new2(msg);
}

static VALUE
mahoro_buffer(self, input)
	VALUE self, input;
{
	const char *msg;
	magic_t cookie = ((struct MagicCookie *) DATA_PTR(self))->cookie;

	if(!(msg = magic_buffer(cookie, StringValuePtr(input),
	                                RSTRING(StringValue(input))->len))) {
		rb_raise(eMahoroError, "failed lookup: %s", magic_error(cookie));
	}

	return rb_str_new2(msg);
}

static VALUE
mahoro_set_flags(self, flags)
	VALUE self, flags;
{
	magic_t cookie = ((struct MagicCookie *) DATA_PTR(self))->cookie;

	return INT2FIX(magic_setflags(cookie, FIX2INT(flags)));
}

static VALUE
mahoro_check(argc, argv, self)
	int argc;
	VALUE *argv, self;
{
	char *path = 0;
	VALUE vpath;
	magic_t cookie = ((struct MagicCookie *) DATA_PTR(self))->cookie;

	switch(rb_scan_args(argc, argv, "01", &vpath)) {
		case 1:
			if(!NIL_P(vpath)) { 
				path = StringValuePtr(vpath);
			}
			break;
	}

	if(!magic_check(cookie, path)) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}

static VALUE
mahoro_compile(klass, path)
	VALUE klass, path;
{
	magic_t cookie = magic_open(MAGIC_NONE);

	if(magic_compile(cookie, StringValuePtr(path))) {
		rb_raise(eMahoroError, "failed compile: %s", magic_error(cookie));
	}

	magic_close(cookie);

	return Qtrue;
}

static VALUE
mahoro_load(self, path)
	VALUE self, path;
{
	magic_t cookie = ((struct MagicCookie *) DATA_PTR(self))->cookie;

	if(magic_load(cookie, StringValuePtr(path))) {
		rb_raise(eMahoroError, "failed load: %s", magic_error(cookie));
	}

	return self;
}

void Init_mahoro(void)
{
	cMahoro      = rb_define_class("Mahoro", rb_cObject);
	eMahoroError = rb_define_class_under(cMahoro, "Error", rb_eStandardError);

	rb_const_set(cMahoro, rb_intern("NONE"), INT2FIX(MAGIC_NONE));
	rb_const_set(cMahoro, rb_intern("DEBUG"), INT2FIX(MAGIC_DEBUG));
	rb_const_set(cMahoro, rb_intern("SYMLINK"), INT2FIX(MAGIC_SYMLINK));
	rb_const_set(cMahoro, rb_intern("COMPRESS"), INT2FIX(MAGIC_COMPRESS));
	rb_const_set(cMahoro, rb_intern("DEVICES"), INT2FIX(MAGIC_DEVICES));
	rb_const_set(cMahoro, rb_intern("MIME"), INT2FIX(MAGIC_MIME));
	rb_const_set(cMahoro, rb_intern("CONTINUE"), INT2FIX(MAGIC_CONTINUE));
	rb_const_set(cMahoro, rb_intern("CHECK"), INT2FIX(MAGIC_CHECK));
	rb_const_set(cMahoro, rb_intern("PRESERVE_ATIME"),
	                      INT2FIX(MAGIC_PRESERVE_ATIME));
	rb_const_set(cMahoro, rb_intern("RAW"), INT2FIX(MAGIC_RAW));
	rb_const_set(cMahoro, rb_intern("ERROR"), INT2FIX(MAGIC_ERROR));

	rb_define_alloc_func(cMahoro, mahoro_allocate);
	rb_define_method(cMahoro, "initialize", mahoro_initialize, -1);
	rb_define_method(cMahoro, "file", mahoro_file, 1);
	rb_define_method(cMahoro, "buffer", mahoro_buffer, 1);
	rb_define_method(cMahoro, "flags=", mahoro_set_flags, 1);
	rb_define_method(cMahoro, "valid?", mahoro_check, -1);
	rb_define_singleton_method(cMahoro, "compile", mahoro_compile, 1);
	rb_define_method(cMahoro, "load", mahoro_load, 1);
}

/* arch-tag: mahoro */
