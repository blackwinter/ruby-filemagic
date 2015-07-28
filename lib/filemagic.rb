begin
  require "filemagic/#{RUBY_VERSION[/\d+.\d+/]}/ruby_filemagic"
rescue LoadError => err
  raise if err.respond_to?(:path) && !err.path
  require 'filemagic/ruby_filemagic'
end

require 'filemagic/version'

class FileMagic

  unless ENV['MAGIC_SILENCE_VERSION_CHECK'] || MAGIC_VERSION == library_version
    warn "#{self} v#{VERSION}: compiled magic version [#{MAGIC_VERSION}] " <<
         "does not match with shared library magic version [#{library_version}]"
  end

  DEFAULT_MAGIC = __FILE__.sub(/\.rb\z/, '/magic.mgc')

  ENV['MAGIC'] ||= DEFAULT_MAGIC unless path

  # Map flag names to their values (:name => Integer).
  FLAGS_BY_SYM = [
    :none,               # No flags
    :debug,              # Turn on debugging
    :symlink,            # Follow symlinks
    :compress,           # Check inside compressed files
    :devices,            # Look at the contents of devices
    :mime_type,          # Return the MIME type
    :continue,           # Return all matches
    :check,              # Print warnings to stderr
    :preserve_atime,     # Restore access time on exit
    :raw,                # Don't convert unprintable chars
    :error,              # Handle ENOENT etc as real errors
    :mime_encoding,      # Return the MIME encoding
    :mime,               # MAGIC_MIME_TYPE | MAGIC_MIME_ENCODING
    :apple,              # Return the Apple creator/type
    :extension,          # Return a /-separated list of extensions
    :compress_transp,    # Check inside compressed files but not report compression
    :nodesc,             # MAGIC_EXTENSION | MAGIC_MIME | MAGIC_APPLE
    :no_check_compress,  # Don't check for compressed files
    :no_check_tar,       # Don't check for tar files
    :no_check_soft,      # Don't check magic entries
    :no_check_apptype,   # Don't check application type
    :no_check_elf,       # Don't check for elf details
    :no_check_text,      # Don't check for text files
    :no_check_cdf,       # Don't check for cdf files
    :no_check_tokens,    # Don't check tokens
    :no_check_encoding,  # Don't check text encodings
    :no_check_builtin,   # No built-in tests; only consult the magic file

    # Defined for backwards compatibility (renamed)
    :no_check_ascii,     # MAGIC_NO_CHECK_TEXT

    # Defined for backwards compatibility; do nothing
    :no_check_fortran,   # Don't check ascii/fortran
    :no_check_troff      # Don't check ascii/troff
  ].inject({}) { |flags, flag|
    const = "MAGIC_#{flag.to_s.upcase}"
    flags.update(flag => const_defined?(const) && const_get(const))
  }

  # Map flag values to their names (Integer => :name).
  FLAGS_BY_INT = FLAGS_BY_SYM.invert.update(0 => :none)

  # Extract "simple" MIME type.
  SIMPLE_RE = %r{([.\w\/-]+)}

  @fm = {}

  class << self

    # Provide a "magic singleton".
    def fm(*flags)
      options = flags.last.is_a?(Hash) ? flags.pop : {}

      if fm = @fm[key = [flags = flags(flags), options]]
        return fm unless fm.closed?
      end

      @fm[key] = new(flags, options)
    end

    # Clear our instance cache.
    def clear!
      @fm.each_value(&:close).clear
    end

    # Just like #new, but takes an optional block, in which case #close
    # is called at the end and the value of the block is returned.
    def open(*flags)
      fm = new(*flags)

      if block_given?
        begin
          yield fm
        ensure
          fm.close
        end
      else
        fm
      end
    end

    # Just a short-cut to #open with the +mime+ flag set.
    def mime(*flags, &block)
      open(:mime, *flags, &block)
    end

    def magic_version(default = MAGIC_VERSION)
      default != '0' ? default :
        user_magic_version ||
        auto_magic_version ||
        [default, 'unknown']
    end

    private

    def user_magic_version(key = 'MAGIC_VERSION')
      [ENV[key], 'user-specified'] if ENV[key]
    end

    def auto_magic_version
      require 'nuggets/file/which'

      if cmd = File.which_command([
        'dpkg-query -f \'${Version}\' -W libmagic-dev',
        'file -v'
      ])
        [%x{#{cmd}}[/\d+\.\d+/], 'auto-detected']
      end
    rescue LoadError
    end

  end

  attr_writer :simplified

  def simplified?
    @simplified
  end

  def io(io, length = 8, rewind = false)
    pos = io.pos if rewind
    buffer(io.read(length))
  ensure
    io.pos = pos if pos
  end

  def fd(fd)
    descriptor(fd.respond_to?(:fileno) ? fd.fileno : fd)
  end

  def inspect
    super.insert(-2, closed? ? ' (closed)' : '')
  end

end
