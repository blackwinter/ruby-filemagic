# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-filemagic}
  s.version = "0.4.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Travis Whitton", "Jens Wille"]
  s.date = %q{2010-09-14}
  s.description = %q{Ruby bindings to the magic(4) library}
  s.email = ["tinymountain@gmail.com", "jens.wille@uni-koeln.de"]
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["README"]
  s.files = ["lib/filemagic/ext.rb", "lib/filemagic/version.rb", "lib/filemagic.rb", "ext/filemagic.h", "ext/filemagic.c", "test/pyfile", "test/mahoro.c", "test/pylink", "test/pyfile-compressed.gz", "test/perl", "test/leaktest.rb", "test/filemagic_test.rb", "test/excel-example.xls", "README", "CHANGELOG", "Rakefile", "TODO", "info/filemagic.rd", "info/example.rb", "ext/extconf.rb"]
  s.homepage = %q{http://ruby-filemagic.rubyforge.org/}
  s.rdoc_options = ["--title", "ruby-filemagic Application documentation", "--main", "README", "--line-numbers", "--inline-source", "--charset", "UTF-8", "--all"]
  s.require_paths = ["lib"]
  s.rubyforge_project = %q{ruby-filemagic}
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{Ruby bindings to the magic(4) library}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
