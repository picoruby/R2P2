require "machine"
require "watchdog"
require "task"
require "shell"
require "spi"

# Not to break symbol table
# FIXME: Fix Sandbox class to remove this workaround
require "vim"

# Setup flash disk
begin
  $shell = Shell.new(clean: true)
  puts "Initializing FLASH disk as the root volume... "
  $shell.setup_root_volume(:flash, label: "R2P2")
  $shell.setup_system_files
  Dir.chdir("/home")
  puts "Available"
rescue => e
  puts "Not available"
  puts "#{e.message} (#{e.class})"
end

begin
  puts "Press 's' to skip running app.mrb or app.rb"
  skip = false
  10.times do
    if IO.getch == "s"
      puts "Skip running app"
      skip = true
      break
    end
    sleep 0.1
  end
  IO.read_nonblock 1024 # discard remaining input
  unless skip
    # Execute /home/app.mrb or /home/app.rb
    if File.exist?("/home/app.mrb")
      Shell::Command.new.exec("/home/app.mrb")
    elsif File.exist?("/home/app.rb")
      Shell::Command.new.exec("/home/app.rb")
    end
  end
  # Start shell if terminal is available
  IO.wait_terminal
  puts "Starting shell...\n\n"
  $shell.show_logo
  $shell.start
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  Watchdog.reboot(1000)
end

