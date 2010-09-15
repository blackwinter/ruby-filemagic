#! /usr/bin/ruby

$:.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))
require 'filemagic'

p FileMagic::VERSION
p FileMagic::MAGIC_VERSION

p FileMagic.new.flags

FileMagic.open(:mime) { |fm|
  p fm.flags
  p fm.file(__FILE__)

  fm.flags = [:raw, :continue]
  p fm.flags
}

fm = FileMagic.new
p fm.flags

mime = FileMagic.mime
p mime.flags

ARGV.each { |file|
  p fm.file(file)
  p fm.file(file, true)

  p mime.file(file)
  p mime.file(file, true)
}
