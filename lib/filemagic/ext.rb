require 'filemagic'

module FileMagic::Ext

  def self.included(base)
    base.class_eval {
      extend ClassMethods
      include InstanceMethods
    }
  end

  module ClassMethods

    def file_type(file, *flags)
      raise NotImplementedError, 'must be implemented by including class'
    end

    def file(file, *flags)
      file_type(file, *flags)
    end

    def mime_type(file, *flags)
      file_type(file, *flags.unshift(:mime))
    end

    alias_method :mime, :mime_type

    def content_type(file, *flags)
      mime_type(file, *flags << { simplified: true })
    end

  end

  module InstanceMethods

    def file_type(*flags)
      self.class.file_type(self, *flags)
    end

    alias_method :file, :file_type

    def mime_type(*flags)
      self.class.mime_type(self, *flags)
    end

    alias_method :mime, :mime_type

    def content_type(*flags)
      self.class.content_type(self, *flags)
    end

  end

end

class File

  include FileMagic::Ext

  def self.file_type(file, *flags)
    FileMagic.fm(*flags).file(file.respond_to?(:path) ? file.path : file)
  rescue FileMagic::FileMagicError
  end

end

class String

  include FileMagic::Ext

  def self.file_type(string, *flags)
    FileMagic.fm(*flags).buffer(string)
  rescue FileMagic::FileMagicError
  end

end

if $0 == __FILE__
  f = __FILE__
  p f

  p File.file_type(f)
  p File.mime_type(f)
  p File.content_type(f)

  f = File.new(f)
  p f

  p f.file_type
  p f.mime_type
  p f.content_type

  s = '#! /usr/bin/ruby'
  p s

  p s.file_type
  p s.mime_type
  p s.content_type
end
