require_relative 'lib/filemagic/version'

begin
  require 'hen'

  file mgc = 'lib/filemagic/magic.mgc' do
    dir = ENV['WITH_CROSS_MAGIC']

    dir && File.exist?(src = File.join(dir, 'magic', File.basename(mgc))) ?
      cp(src, mgc) : File.binwrite(mgc, "\x1C\x04\x1E\xF1\x0C".ljust(248, "\0"))
  end

  Hen.lay! {{
    gem: {
      name:        %q{ruby-filemagic},
      version:     FileMagic::VERSION,
      summary:     'Ruby bindings to the magic(4) library',
      authors:     ['Travis Whitton', 'Jens Wille'],
      email:       'jens.wille@gmail.com',
      license:     %q{Ruby},
      homepage:    :blackwinter,
      local_files: [mgc],
      extension:   {
        with_cross_gnurx: lambda { |dir| [dir] },
        with_cross_magic: lambda { |dir| [src =
          File.join(dir, 'src'), File.join(src, '.libs')] }
      },

      required_ruby_version: '>= 1.9.3'
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem. (#{err})"
end
