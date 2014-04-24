# -*- encoding: utf-8 -*-
# stub: ruby-filemagic 0.5.2 ruby lib
# stub: ext/filemagic/extconf.rb

Gem::Specification.new do |s|
  s.name = "ruby-filemagic"
  s.version = "0.5.2"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Travis Whitton", "Jens Wille"]
  s.date = "2014-04-24"
  s.description = "Ruby bindings to the magic(4) library"
  s.email = ["tinymountain@gmail.com", "jens.wille@gmail.com"]
  s.extensions = ["ext/filemagic/extconf.rb"]
  s.extra_rdoc_files = ["README", "ChangeLog", "ext/filemagic/filemagic.c"]
  s.files = ["ChangeLog", "README", "Rakefile", "TODO", "ext/filemagic/extconf.rb", "ext/filemagic/filemagic.c", "ext/filemagic/filemagic.h", "info/example.rb", "info/filemagic.rd", "lib/filemagic.rb", "lib/filemagic/ext.rb", "lib/filemagic/version.rb", "lib/ruby-filemagic.rb", "test/excel-example.xls", "test/filemagic_test.rb", "test/leaktest.rb", "test/mahoro.c", "test/perl", "test/pyfile", "test/pyfile-compressed.gz", "test/pylink"]
  s.homepage = "http://github.com/blackwinter/ruby-filemagic"
  s.licenses = ["Ruby"]
  s.post_install_message = "\nruby-filemagic-0.5.2 [2014-04-24]:\n\n* Use MAGIC_VERSION if available.\n* Made tests more robust.\n\n"
  s.rdoc_options = ["--title", "ruby-filemagic Application documentation (v0.5.2)", "--charset", "UTF-8", "--line-numbers", "--all", "--main", "README"]
  s.rubygems_version = "2.2.2.x"
  s.summary = "Ruby bindings to the magic(4) library"

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<hen>, [">= 0"])
      s.add_development_dependency(%q<rake>, [">= 0"])
      s.add_development_dependency(%q<test-unit>, [">= 0"])
    else
      s.add_dependency(%q<hen>, [">= 0"])
      s.add_dependency(%q<rake>, [">= 0"])
      s.add_dependency(%q<test-unit>, [">= 0"])
    end
  else
    s.add_dependency(%q<hen>, [">= 0"])
    s.add_dependency(%q<rake>, [">= 0"])
    s.add_dependency(%q<test-unit>, [">= 0"])
  end
end
