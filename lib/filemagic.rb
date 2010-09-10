require 'filemagic.so'

class FileMagic

  # Map flag names to their values (:name => Integer).
  FLAGS_BY_SYM = [
    :none,               # No flags
    :debug,              # Turn on debugging
    :symlink,            # Follow symlinks
    :compress,           # Check inside compressed files
    :devices,            # Look at the contents of devices
    :mime_type,          # Return only the MIME type
    :continue,           # Return all matches
    :check,              # Print warnings to stderr
    :preserve_atime,     # Restore access time on exit
    :raw,                # Don't translate unprint chars
    :error,              # Handle ENOENT etc as real errors
    :mime_encoding,      # Return only the MIME encoding
    :mime,               # MAGIC_MIME_TYPE | MAGIC_MIME_ENCODING
    :no_check_compress,  # Don't check for compressed files
    :no_check_tar,       # Don't check for tar files
    :no_check_soft,      # Don't check magic entries
    :no_check_apptype,   # Don't check application type
    :no_check_elf,       # Don't check for elf details
    :no_check_ascii,     # Don't check for ascii files
    :no_check_troff,     # Don't check ascii/troff
    :no_check_tokens,    # Don't check ascii/tokens
    :no_check_fortran    # Don't check ascii/fortran
  ].inject({}) { |flags, flag|
    const = "MAGIC_#{flag.to_s.upcase}"
    flags.update(flag => const_defined?(const) && const_get(const))
  }

  # Map flag values to their names (Integer => :name).
  FLAGS_BY_INT = FLAGS_BY_SYM.invert.update(0 => :none)

  attr_writer :simplified

  @fm = Hash.new { |fm, flags|
    fm.has_key?(key = flags.to_s) ? fm[key] : fm[key] = new(*flags)
  }

  class << self

    # Provide a "magic singleton" ;-)
    def fm(*flags)
      @fm.delete(flags.to_s) if @fm[flags].closed?
      @fm[flags]
    end

    # Clear our instance cache.
    def clear!
      @fm.each_value { |fm| fm.close }
      @fm.clear
    end

    # Allow shorter names for flags; now also takes options.
    def new(*flags)
      warn "warning: FileMagic::new() does not take block; use FileMagic::open() instead" if block_given?

      options = flags.last.is_a?(Hash) ? flags.pop : {}

      fm = fm_new(build_flags(*flags))

      options.each { |option, value|
        if fm.respond_to?(method = "#{option}=")
          fm.send(method, value)
        else
          raise ArgumentError, "illegal option: #{option.inspect}"
        end
      }

      fm
    end

    # Just like #new, but takes an optional block, in which case #close is called at the end.
    def open(*flags)
      fm = new(*flags)

      if block_given?
        begin
          yield fm
        ensure
          fm.close
        end
      end

      fm
    end

    # Just a short-cut ;-)
    def mime(*flags, &block)
      open(:mime, *flags, &block)
    end

    # Build the actual flags from the named flags passed as arguments.
    def build_flags(*flags)
      int_flags = *flags

      unless int_flags.is_a?(Integer)
        int_flags = MAGIC_NONE

        flags.flatten.each { |flag|
          if value = flag.is_a?(Integer) ? flag : FLAGS_BY_SYM[flag.to_sym]
            int_flags |= value
          else
            err = value.nil? ? 'no such flag' : 'flag not available'
            raise ArgumentError, "#{err}: #{flag}"
          end
        }
      end

      int_flags
    end

  end

  # The actual processing happens in #new.
  def initialize(*flags)
    fm_initialize(*flags)

    @flags  = flags
    @closed = false
  end

  def close
    return if closed?

    fm_close
    @closed = true
  end

  # Optionally cut off additional information after mime-type.
  def file(file, simplified = simplified?)
    simplify_mime(fm_file(file), simplified)
  end

  # Returns the flags as integer.
  def int_flags
    @flags.first
  end

  # Returns the flags as array of symbols.
  def flags
    int, flags, log2 = int_flags, [], Math.log(2)

    while int > 0
      flags << FLAGS_BY_INT[flag = 2 ** Math.log(int).div(log2)]
      int -= flag
    end

    flags.reverse
  end

  # Optionally cut off additional information after mime-type.
  def buffer(buffer, simplified = simplified?)
    simplify_mime(fm_buffer(buffer), simplified)
  end

  def setflags(*flags)
    @flags = [self.class.build_flags(*flags)]
    fm_setflags(int_flags)
  end

  alias_method :flags=, :setflags

  def check(file = "\0")
    fm_check(file)
  end

  alias_method :valid?, :check

  def compile(file)
    fm_compile(file)
  end

  def simplified?
    @simplified
  end

  def closed?
    @closed
  end

  def inspect
    str = super
    str.insert(-2, ' (closed)') if closed?
    str
  end

  private

  def simplify_mime(mime, simplified = true)
    simplified ? mime[/([.\w\/-]+)/, 1].downcase : mime
  end

end
