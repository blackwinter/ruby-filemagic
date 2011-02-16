require File.expand_path(%q{../lib/filemagic/version}, __FILE__)

begin
  require 'hen'

  Hen.lay! {{
    :rubyforge => {
      :package => %q{ruby-filemagic}
    },

    :gem => {
      :version     => FileMagic::VERSION,
      :summary     => 'Ruby bindings to the magic(4) library',
      :authors     => ['Travis Whitton', 'Jens Wille'],
      :email       => ['tinymountain@gmail.com', 'jens.wille@uni-koeln.de'],
      :extra_files => FileList['info/*'].to_a
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem first. (#{err})"
end
