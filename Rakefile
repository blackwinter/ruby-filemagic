require File.expand_path(%q{../lib/filemagic/version}, __FILE__)

begin
  require 'hen'

  Hen.lay! {{
    gem: {
      name:         %q{ruby-filemagic},
      version:      FileMagic::VERSION,
      summary:      'Ruby bindings to the magic(4) library',
      authors:      ['Travis Whitton', 'Jens Wille'],
      email:        'jens.wille@gmail.com',
      license:      %q{Ruby},
      homepage:     :blackwinter,
      extra_files:  FileList['info/*'].to_a,
      dependencies: %w[],
      extension:    {
        name:          'ruby_filemagic',
        ext_name:      'filemagic',
        cross_compile: false
      },

      required_ruby_version: '>= 1.9.3'
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem first. (#{err})"
end
