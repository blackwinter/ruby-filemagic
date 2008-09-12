=begin

= FileMagic extension module SEE ALSO libmagic(3), file(1) and magic(4)
== FileMagic
=== Constants
: MAGIC_NONE
     No flags
: MAGIC_DEBUG
     Turn on debuggin
: MAGIC_SYMLINK
     Follow symlinks
: MAGIC_COMPRESS
     Check inside compressed files
: MAGIC_DEVICES
     Look at the contents of devices
: MAGIC_MIME
     Return a mime string
: MAGIC_CONTINUE
     Return all matches, not just the first
: MAGIC_CHECK
     Print warnings to stderr

=== Methods
: file(filename)
    returns a textual description of the contents of the filename argument
: buffer(string)
    returns a textual description of the contents of the string argument
: check(filename)
    checks the validity of entries in the colon separated database files
    passed in as filename
: compile(filename)
    compile the the colon separated list of database files passed in as
    filename
: close()
    closes the magic database and frees any memory allocated. if memory
    is a concern, use this.
