# -*- encoding: utf-8 -*-
# stub: ruby-filemagic 0.5.0 ruby lib

Gem::Specification.new do |s|
  s.name = "ruby-filemagic"
  s.version = "0.5.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Travis Whitton", "Jens Wille"]
  s.date = "2013-12-19"
  s.description = "Ruby bindings to the magic(4) library"
  s.email = ["tinymountain@gmail.com", "jens.wille@gmail.com"]
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["README", "ChangeLog", "ext/filemagic.c"]
  s.files = ["lib/filemagic.rb", "lib/filemagic/ext.rb", "lib/filemagic/version.rb", "info/example.rb", "info/filemagic.rd", "ChangeLog", "README", "Rakefile", "TODO", "ext/extconf.rb", "ext/filemagic.c", "ext/filemagic.h", "test/excel-example.xls", "test/filemagic_test.rb", "test/leaktest.rb", "test/mahoro.c", "test/perl", "test/pyfile", "test/pyfile-compressed.gz", "test/pylink"]
  s.homepage = "http://github.com/blackwinter/ruby-filemagic"
  s.licenses = ["Ruby"]
  s.post_install_message = "\nruby-filemagic-0.5.0 [2013-12-19]:\n\n* Removed usage of +RARRAY_PTR+ (cf. Ruby bug\n  {#8399}[https://bugs.ruby-lang.org/issues/8399]).\n* Housekeeping.\n\n"
  s.rdoc_options = ["--title", "ruby-filemagic Application documentation (v0.5.0)", "--charset", "UTF-8", "--line-numbers", "--all", "--main", "README"]
  s.require_paths = ["lib"]
  s.rubygems_version = "2.1.11"
  s.summary = "Ruby bindings to the magic(4) library"
end
