require 'mkmf'

$CFLAGS << ' -Wall' if ENV['WALL']
$LDFLAGS << ' -static-libgcc'

dir_config('magic')
dir_config('gnurx')

have_library('gnurx')

if have_library('magic', 'magic_open') && have_header('magic.h')
  have_header('file/patchlevel.h')
  create_makefile('filemagic/ruby_filemagic')
else
  abort '*** ERROR: missing required library to compile this module'
end
