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
    :apple,              # Return the Apple creator and type
    :no_check_compress,  # Don't check for compressed files
    :no_check_tar,       # Don't check for tar files
    :no_check_soft,      # Don't check magic entries
    :no_check_apptype,   # Don't check application type
    :no_check_elf,       # Don't check for elf details
    :no_check_text,      # Don't check for text files
    :no_check_cdf,       # Don't check for cdf files
    :no_check_tokens,    # Don't check ascii/tokens
    :no_check_encoding,  # Don't check text encodings
    :no_check_ascii,     # MAGIC_NO_CHECK_TEXT
    :no_check_fortran,   # Don't check ascii/fortran
    :no_check_troff      # Don't check ascii/troff
  ].inject({}) { |flags, flag|
    const = "MAGIC_#{flag.to_s.upcase}"
    flags.update(flag => const_defined?(const) && const_get(const))
  }

  # Map flag values to their names (Integer => :name).
  FLAGS_BY_INT = FLAGS_BY_SYM.invert.update(0 => :none)

  # Extract "simple" MIME type
  SIMPLE_RE = %r{([.\w\/-]+)}

  @fm = Hash.new { |fm, flags|
    fm.has_key?(key = flags.to_s) ? fm[key] : fm[key] = new(*flags)
  }

  class << self

    # Provide a "magic singleton".
    def fm(*flags)
      @fm.delete(flags.to_s) if @fm[flags].closed?
      @fm[flags]
    end

    # Clear our instance cache.
    def clear!
      @fm.each_value { |fm| fm.close }
      @fm.clear
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

  end

  attr_writer :simplified

  def simplified?
    @simplified
  end

  def inspect
    super.insert(-2, closed? ? ' (closed)' : '')
  end

end
