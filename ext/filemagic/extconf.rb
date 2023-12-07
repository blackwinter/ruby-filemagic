require 'mkmf'

$CFLAGS << ' -Wall' if ENV['WALL']
$LDFLAGS << ' -static-libgcc' if RUBY_PLATFORM =~ /cygwin|mingw|mswin/

dir_config('magic')
dir_config('gnurx')

if have_library('magic', 'magic_open')
  if have_header('magic.h')
    have_library('gnurx')
    have_func('magic_version')
    have_header('file/patchlevel.h')
    create_makefile('filemagic/ruby_filemagic')
  else
    abort '*** ERROR: missing magic.h (required to compile this module)'
  end
else
  abort '*** ERROR: missing magic library (required to build this module)'
end
