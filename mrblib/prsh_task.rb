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

begin
IO.wait_and_clear
  Shell.new.start(:prsh)
rescue => e
  puts "#{e.message} (#{e.class})"
  retry
end
