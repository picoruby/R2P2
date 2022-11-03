require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

File = MyFile
Dir = MyDir

def exit
  raise # to restart
end

IO.wait_and_clear
FAT._setup(0) # Workaround until Flash ROM works

begin
  IO.wait_and_clear
  Shell.new.start(:prsh)
rescue => e
  puts "#{e.message} (#{e.class})"
  retry
end
