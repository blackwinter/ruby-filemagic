#! /usr/bin/ruby

$:.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))
require 'filemagic'

puts FileMagic.new.flags

FileMagic.open(:mime) { |fm|
  puts fm.flags
  puts fm.file(__FILE__)

  fm.setflags(:raw, :continue)
  puts fm.flags
}

fm = FileMagic.new
puts fm.flags

mime = FileMagic.mime
puts mime.flags

ARGV.each { |file|
  puts fm.file(file)
  puts fm.file(file, true)

  puts mime.file(file)
  puts mime.file(file, true)
}
