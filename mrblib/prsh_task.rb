require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

#flash = FAT.new("1")
#VFS.mount(flash, "/")
#File = MyFile
#Dir = MyDir

IO.wait_and_clear

def exit
  # do nothing
end

begin
  Shell.new.start(:prsh)
rescue => e
  puts "#{e.message} (#{e.class})"
  retry
end
