require "shell"

def exit
  raise # to restart
end

ENV['PATH'] = "/bin"
ENV['HOME'] = "/home"

begin
  Shell.setup(:flash, "FLASH")
  IO.wait_and_clear
  unless VFS::VOLUMES.any?{|v| v[:driver].device == :sd }
    VFS.mount FAT.new(:sd, "SD"), "/sd"
  end
  Shell.new.start(:shell)
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  retry
end

