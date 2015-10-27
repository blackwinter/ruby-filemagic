require 'test/unit'
require 'filemagic'

class TestFileMagic < Test::Unit::TestCase

  magic_version, origin = FileMagic.magic_version
  MAGIC_VERSION = magic_version.to_f

  warn <<-EOT

libmagic version: #{MAGIC_VERSION}#{" (#{origin})" if origin}
magic file from #{FileMagic.path}

  EOT

  def test_file
    fm = FileMagic.new(FileMagic::MAGIC_NONE)

    python_script = match_version(
      0    => 'a python script, ASCII text executable',
      5.11 => 'Python script, ASCII text executable'
    )

    res = fm.file(path_to('pyfile'))
    assert_equal(python_script, res)

    if File.symlink?(path_to('pylink'))
      res = fm.file(path_to('pylink'))
      assert_equal(match_version(
        0    => "symbolic link to `pyfile'",
        5.22 => 'symbolic link to pyfile'
      ), res.strip)
    end

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK)

    res = fm.file(path_to('pylink'))
    assert_equal(python_script, res)

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK | FileMagic::MAGIC_MIME)

    res = fm.file(path_to('pylink'))
    assert_equal('text/plain; charset=us-ascii', res)

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_COMPRESS)

    res = fm.file(path_to('pyfile-compressed.gz'))
    gzip_compressed = 'gzip compressed data, was "pyfile-compressed"'
    assert_match(Gem.win_platform? ? /^#{gzip_compressed}/ :
                 /^#{python_script} \(#{gzip_compressed}/, res)

    fm.close
  end

  def test_buffer
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.buffer("#!/bin/sh\n")
    fm.close
    assert_equal('POSIX shell script, ASCII text executable', res)
  end

  def test_descriptor
    fm = FileMagic.new(FileMagic::MAGIC_NONE)

    python_script = match_version(
      0    => 'a python script, ASCII text executable',
      5.11 => 'Python script, ASCII text executable'
    )

    fd_for('pyfile') { |fd|
      res = fm.descriptor(fd)
      assert_equal(python_script, res)
    }

    if File.symlink?(path_to('pylink'))
      fd_for('pylink') { |fd|
        res = fm.descriptor(fd)
        assert_equal(python_script, res.strip)
      }
    end

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK)

    fd_for('pylink') { |fd|
      res = fm.descriptor(fd)
      assert_equal(python_script, res)
    }

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_SYMLINK | FileMagic::MAGIC_MIME)

    fd_for('pylink') { |fd|
      res = fm.descriptor(fd)
      assert_equal('text/plain; charset=us-ascii', res)
    }

    fm.close
    fm = FileMagic.new(FileMagic::MAGIC_COMPRESS)

    fd_for('pyfile-compressed.gz') { |fd|
      res = fm.descriptor(fd)
      gzip_compressed = 'gzip compressed data, was "pyfile-compressed"'
      assert_match(Gem.win_platform? ? /^#{gzip_compressed}/ :
                   /^#{python_script} \(#{gzip_compressed}/, res)
    }

    fm.close
  end

  def test_check
    return if Gem.win_platform?
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = silence_stderr { fm.check(path_to('perl')) }
    fm.close
    assert(res)
  end

  def test_check_compiled
    return if MAGIC_VERSION <= 5.09
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = silence_stderr { fm.check(path_to('perl.mgc')) }
    fm.close
    assert(res)
  end

  def test_compile
    assert(File.writable?('.'), "can't write to current directory")
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    res = fm.compile(path_to('perl'))
    fm.close
    assert(res)
    File.unlink(path_to('perl.mgc', '.'))
  end

  def test_block
    block_fm = nil
    res = FileMagic.open(FileMagic::MAGIC_NONE) { |fm|
      block_fm = fm
      fm.file(path_to('pyfile'))
    }
    assert_equal(match_version(
      0    => 'a python script, ASCII text executable',
      5.11 => 'Python script, ASCII text executable'
    ), res)
    assert block_fm.closed?
  end

  def test_flags_to_int
    assert_raise(TypeError) { FileMagic.flags(0) }
    assert_equal(0, FileMagic.flags([FileMagic::MAGIC_NONE]))
    assert_equal(0, FileMagic.flags([:none]))
    assert_equal(0, FileMagic.flags([]))
    assert_equal(1072, FileMagic.flags([:mime, :continue]))
  end

  def test_setflags
    fm = FileMagic.new(FileMagic::MAGIC_NONE)
    assert_equal([], fm.flags)
    fm.flags = FileMagic::MAGIC_SYMLINK
    assert_equal([:symlink], fm.flags)
    fm.close
  end

  def test_abbr
    fm = FileMagic.new(:mime, :continue)
    assert_equal([:mime_type, :continue, :mime_encoding], fm.flags)
    fm.flags = :symlink
    assert_equal([:symlink], fm.flags)
    fm.close
  end

  def test_close
    fm = FileMagic.new
    fm.close
    assert fm.closed?
    fm.close
    assert fm.closed?
  end

  # tests adapted from mahoro:

  def test_mahoro_file
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_NONE
    assert_equal(match_version(
      0    => 'ASCII C program text',
      5.11 => 'C source, ASCII text'
    ), fm.file(path_to('mahoro.c')))
  end

  def test_mahoro_mime_file
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_MIME
    assert_equal('text/x-c; charset=us-ascii', fm.file(path_to('mahoro.c')))
  end

  def test_mahoro_buffer
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_NONE
    assert_equal(match_version(
      0    => 'ASCII C program text',
      5.11 => 'C source, ASCII text'
    ), fm.buffer(File.read(path_to('mahoro.c'))))
  end

  def test_mahoro_mime_buffer
    fm = FileMagic.new
    fm.flags = FileMagic::MAGIC_MIME
    assert_equal('text/x-c; charset=us-ascii', fm.buffer(File.read(path_to('mahoro.c'))))
  end

  def test_mahoro_valid
    fm = FileMagic.new
    assert(silence_stderr { fm.valid? }, 'Default database was not valid.')
  end

  # test abbreviating mime types

  def test_abbrev_mime_type
    fm = FileMagic.mime

    refute fm.simplified?
    assert_equal('text/plain; charset=us-ascii', fm.file(path_to('perl')))

    fm.simplified = true
    assert fm.simplified?
    assert_equal('text/plain', fm.file(path_to('perl')))
    assert_equal(match_version(
      0    => 'application/vnd.ms-office',
      5.11 => 'application/msword',
      5.14 => 'application/vnd.ms-office'
    ), fm.file(path_to('excel-example.xls')))
  end

  def test_singleton
    fm1 = FileMagic.fm
    assert_equal(fm1, FileMagic.fm)

    refute fm1.simplified?
    assert_equal('ASCII text', fm1.file(path_to('perl')))

    fm2 = FileMagic.fm(:mime)
    assert_equal(fm2, FileMagic.fm(:mime))
    refute_equal(fm2, fm1)

    refute fm2.simplified?
    assert_equal('text/plain; charset=us-ascii', fm2.file(path_to('perl')))

    fm3 = FileMagic.fm(:mime, simplified: true)
    assert_equal(fm3, FileMagic.fm(:mime, simplified: true))
    refute_equal(fm3, fm2)
    refute_equal(fm3, fm1)

    assert fm3.simplified?
    assert_equal('text/plain', fm3.file(path_to('perl')))
  end

  # utility methods:

  def path_to(file, dir = File.dirname(__FILE__))
    File.join(dir, file)
  end

  def fd_for(file)
    File.open(path_to(file)) { |f| yield f.fileno }
  rescue Errno::EBADF => err
    warn err.to_s
  end

  def silence_stderr
    require 'nuggets/io/redirect'
    $stderr.redirect { yield }
  rescue LoadError
    yield
  end

  def match_version(versions)
    versions.sort_by { |k,| -k }.find { |k,| k <= MAGIC_VERSION }.last
  end

end
