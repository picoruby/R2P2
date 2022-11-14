require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

ENV = {}
ARGV = []

File = MyFile
Dir = MyDir

def exit
  raise # to restart
end

IO.wait_and_clear
FAT._setup(0) # Workaround until Flash ROM works

begin
  IO.wait_and_clear
  Shell.new.start(:shell)
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  retry
end
