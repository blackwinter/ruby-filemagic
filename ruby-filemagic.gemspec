# -*- encoding: utf-8 -*-
# stub: ruby-filemagic 0.7.3 ruby lib
# stub: ext/filemagic/extconf.rb

Gem::Specification.new do |s|
  s.name = "ruby-filemagic".freeze
  s.version = "0.7.3"

  s.required_rubygems_version = Gem::Requirement.new(">= 0".freeze) if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib".freeze]
  s.authors = ["Travis Whitton".freeze, "Jens Wille".freeze]
  s.date = "2022-01-07"
  s.description = "Ruby bindings to the magic(4) library".freeze
  s.email = "jens.wille@gmail.com".freeze
  s.extensions = ["ext/filemagic/extconf.rb".freeze]
  s.extra_rdoc_files = ["README".freeze, "ChangeLog".freeze, "ext/filemagic/filemagic.c".freeze]
  s.files = ["CONTRIBUTING.md".freeze, "ChangeLog".freeze, "Dockerfile".freeze, "README".freeze, "Rakefile".freeze, "TODO".freeze, "ext/filemagic/extconf.rb".freeze, "ext/filemagic/filemagic.c".freeze, "ext/filemagic/filemagic.h".freeze, "lib/filemagic.rb".freeze, "lib/filemagic/ext.rb".freeze, "lib/filemagic/magic.mgc".freeze, "lib/filemagic/version.rb".freeze, "lib/ruby-filemagic.rb".freeze, "test/excel-example.xls".freeze, "test/filemagic_test.rb".freeze, "test/leaktest.rb".freeze, "test/mahoro.c".freeze, "test/perl".freeze, "test/perl.mgc".freeze, "test/pyfile".freeze, "test/pyfile-compressed.gz".freeze, "test/pylink".freeze]
  s.homepage = "http://github.com/blackwinter/ruby-filemagic".freeze
  s.licenses = ["Ruby".freeze]
  s.post_install_message = "\nruby-filemagic-0.7.3 [2022-01-07]:\n\n* Dockerfile to build native extension (pull request #26 by Pavel Lobashov).\n* Include paths for ARM-based Apple Macs (Apple Silicon) (pull request #35 by\n  @545ch4).\n\n".freeze
  s.rdoc_options = ["--title".freeze, "ruby-filemagic Application documentation (v0.7.3)".freeze, "--charset".freeze, "UTF-8".freeze, "--line-numbers".freeze, "--all".freeze, "--main".freeze, "README".freeze]
  s.required_ruby_version = Gem::Requirement.new(">= 1.9.3".freeze)
  s.rubygems_version = "3.2.5".freeze
  s.summary = "Ruby bindings to the magic(4) library".freeze

  if s.respond_to? :specification_version then
    s.specification_version = 4
  end

  if s.respond_to? :add_runtime_dependency then
    s.add_development_dependency(%q<hen>.freeze, ["~> 0.9", ">= 0.9.1"])
    s.add_development_dependency(%q<rake>.freeze, [">= 0"])
    s.add_development_dependency(%q<rake-compiler>.freeze, [">= 0"])
    s.add_development_dependency(%q<test-unit>.freeze, [">= 0"])
  else
    s.add_dependency(%q<hen>.freeze, ["~> 0.9", ">= 0.9.1"])
    s.add_dependency(%q<rake>.freeze, [">= 0"])
    s.add_dependency(%q<rake-compiler>.freeze, [">= 0"])
    s.add_dependency(%q<test-unit>.freeze, [">= 0"])
  end
end
