require '../filemagic'

# watch -n 1 'ps aux | grep leaktest'

def do_file(filename)
  fm = FileMagic.new(0)
  file = fm.file(filename)
  fm.close
  return file
end

loop do
  puts do_file($0)
  sleep 1
end
