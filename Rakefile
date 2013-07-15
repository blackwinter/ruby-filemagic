require File.expand_path(%q{../lib/filemagic/version}, __FILE__)

begin
  require 'hen'

  Hen.lay! {{
    :gem => {
      :name         => %q{ruby-filemagic},
      :version      => FileMagic::VERSION,
      :summary      => 'Ruby bindings to the magic(4) library',
      :authors      => ['Travis Whitton', 'Jens Wille'],
      :email        => ['tinymountain@gmail.com', 'jens.wille@gmail.com'],
      :license      => %q{Ruby},
      :homepage     => :blackwinter,
      :extra_files  => FileList['info/*'].to_a,
      :dependencies => %w[],
      :extension    => {
        :name          => 'filemagic',
        :ext_dir       => 'ext',
        :lib_dir       => 'lib',
        :cross_compile => false
      }
    }
  }}
rescue LoadError => err
  warn "Please install the `hen' gem first. (#{err})"
end
