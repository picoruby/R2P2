require "shell"

def exit
  raise # to restart
end

ENV['PATH'] = "/bin"
ENV['HOME'] = "/home"

begin
  Shell.setup(:flash)
  IO.wait_and_clear
  Shell.new.start(:shell)
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  retry
end
