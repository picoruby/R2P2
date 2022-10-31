require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

#flash = FAT.new("1")
#VFS.mount(flash, "/")
#File = MyFile
#Dir = MyDir

# Wait until CDC becomes stable
3.times do |i|
  sleep 1
  puts "#{2 - i}"
end

Shell.new.start(:prsh)

