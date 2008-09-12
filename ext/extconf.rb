require 'mkmf'

dir_config("magic")
if have_library("magic", "magic_open") and have_header("magic.h")
  create_makefile("filemagic")
else
  print "***  ERROR: missing required library to compile this module\n"
end
