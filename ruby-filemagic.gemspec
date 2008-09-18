Gem::Specification.new do |s|
  s.name = %q{ruby-filemagic}
  s.version = "0.2.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Travis Whitton", "Jens Wille"]
  s.date = %q{2008-09-18}
  s.description = %q{Ruby bindings to the magic(4) library}
  s.email = ["tinymountain@gmail.com", "jens.wille@uni-koeln.de"]
  s.extensions = ["ext/extconf.rb"]
  s.extra_rdoc_files = ["README"]
  s.files = ["lib/filemagic/ext.rb", "lib/filemagic/version.rb", "lib/filemagic.rb", "ext/filemagic.c", "test/pyfile", "test/pyfile-compressed.gz", "test/perl", "test/leaktest.rb", "test/mahoro.c", "test/filemagic_test.rb", "test/pylink", "README", "CHANGELOG", "TODO", "Rakefile", "info/filemagic.rd", "info/example.rb", "ext/extconf.rb"]
  s.has_rdoc = true
  s.homepage = %q{http://ruby-filemagic.rubyforge.org/}
  s.rdoc_options = ["--line-numbers", "--main", "README", "--all", "--inline-source", "--title", "ruby-filemagic Application documentation", "--charset", "UTF-8"]
  s.require_paths = ["lib"]
  s.rubyforge_project = %q{ruby-filemagic}
  s.rubygems_version = %q{1.2.0}
  s.summary = %q{Ruby bindings to the magic(4) library}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 2

    if current_version >= 3 then
    else
    end
  else
  end
end
