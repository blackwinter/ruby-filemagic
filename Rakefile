require_relative 'lib/filemagic/version'

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
  warn "Please install the `hen' gem. (#{err})"
end
