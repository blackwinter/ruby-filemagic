require "test/unit"
require "../filemagic"

class TestFileMagic < Test::Unit::TestCase
  def test_file
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.file("pyfile")
    assert_equal("a python script text executable", res)
    res = fm.file("pylink")
    assert_equal("symbolic link to `pyfile'", res)
    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK)
    res = fm.file("pylink")
    assert_equal("a python script text executable", res)
    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK | FileMagic::MAGIC_MIME)
    res = fm.file("pylink")
    assert_equal("text/plain; charset=us-ascii", res)
    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_COMPRESS)
    res = fm.file("pyfile-compressed.gz")
    assert_equal("a python script text executable (gzip compressed data, " +
                 'was "pyfile-compressed", from Unix)', res)
    fm.close
  end

  def test_buffer
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.buffer("#!/bin/sh\n")
    fm.close
    assert_equal("a /bin/sh script text executable", res)
  end

  def test_check
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.check("perl")
    fm.close
    assert_equal(res, 0)
  end

  def test_compile
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.compile("perl")
    fm.close
    assert_equal(res, 0)
    File.unlink("perl.mgc")
  end
end
