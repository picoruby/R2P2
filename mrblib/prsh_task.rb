require "sandbox"
require "shell"
require "filesystem-fat"
require "vfs"
require "vim"

File = MyFile
Dir = MyDir

IO.wait_and_clear

fat = FAT.new(:flash)
retry_count = 0
begin
  VFS.mount(fat, "/")
rescue => e
  puts e.message
  fat.mkfs
  retry_count += 1
  retry if retry_count == 1
  raise e
end

File.open("test.txt", "w") do |f|
  f.puts "hello"
  f.puts "world"
end

def exit
  raise # to restart
end

begin
  Shell.new.start(:prsh)
rescue => e
  puts "#{e.message} (#{e.class})"
  retry
end
