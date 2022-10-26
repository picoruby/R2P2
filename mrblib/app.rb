require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

flash = FAT.new("1")
VFS.mount(flash, "/")
File = MyFile
Dir = MyDir

Shell.new.start(:prsh)
