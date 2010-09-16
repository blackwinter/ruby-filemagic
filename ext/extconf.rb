require 'mkmf'

$CFLAGS << ' -Wall' if ENV['WALL']
dir_config('magic')

if have_library('magic', 'magic_open') && have_header('magic.h')
  have_header('file/patchlevel.h')
  create_makefile('filemagic')
else
  abort '*** ERROR: missing required library to compile this module'
end
