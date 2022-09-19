require 'mkmf'

hdr_dirs = lib_dirs = nil

if RbConfig::CONFIG['host_os'] =~ /darwin/
  unless with_config('magic-dir') || with_config('magic-include')
    hdr_dirs = [
      '/opt/local/include', # MacPorts
      '/usr/local/include', # compiled from source and Homebrew
      '/opt/homebrew/include', # compiled from source and Homebrew (ARM based Macs)
      '/usr/include',       # system
    ]
  end

  unless with_config('magic-dir') || with_config('magic-lib')
    lib_dirs = [
      '/opt/local/lib', # MacPorts
      '/usr/local/lib', # compiled from source and Homebrew
      '/opt/homebrew/lib', # compiled from source and Homebrew (ARM based Macs)
      '/usr/lib',       # system
    ]
  end
end

$CFLAGS << ' -Wall' if ENV['WALL']
$LDFLAGS << ' -static-libgcc' if RUBY_PLATFORM =~ /cygwin|mingw|mswin/

dir_config('magic', hdr_dirs, lib_dirs)
dir_config('gnurx', hdr_dirs, lib_dirs)

have_library('gnurx')

if have_library('magic', 'magic_open') && have_header('magic.h')
  have_func('magic_version')
  have_header('file/patchlevel.h')
  create_makefile('filemagic/ruby_filemagic')
else
  abort '*** ERROR: missing required header and/or library to compile this module'
end
