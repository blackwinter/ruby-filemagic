require %q{./lib/filemagic/version}

begin
  require 'hen'

  Hen.lay! {{
    :rubyforge => {
      :package  => 'ruby-filemagic',
      :project  => 'ruby-filemagic',
      :rdoc_dir => nil
    },

    :gem => {
      :version     => FileMagic::VERSION,
      :summary     => 'Ruby bindings to the magic(4) library',
      :authors     => ['Travis Whitton', 'Jens Wille'],
      :email       => ['tinymountain@gmail.com', 'jens.wille@uni-koeln.de'],
      :homepage    => 'http://ruby-filemagic.rubyforge.org/',
      :files       => FileList['lib/**/*.rb', 'ext/**/*.[ch]', 'test/*'].to_a,
      :extensions  => FileList['ext/**/extconf.rb'].to_a,
      :extra_files => FileList['[A-Z]*', 'info/*'].to_a
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem first. (#{err})"
end
